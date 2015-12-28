// ExponentialChannelNoiseService.cxx

#include "dune/DetSim/Service/ExponentialChannelNoiseService.h"
#include "Utilities/DetectorProperties.h"
#include "Utilities/LArFFT.h"
#include "Geometry/Geometry.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "artextensions/SeedService/SeedService.hh"
#include "CLHEP/Random/RandFlat.h"
#include "TH1F.h"

using std::ostream;
using std::endl;
using std::string;

#undef UseSeedService

//**********************************************************************

ExponentialChannelNoiseService::
ExponentialChannelNoiseService(fhicl::ParameterSet const& pset, art::ActivityRegistry&):
  fNoiseHist(nullptr), fNoiseChanHist(nullptr),
  m_pran(nullptr) {
  fNoiseFactZ        = pset.get<double>("NoiseFactZ");
  fNoiseWidthZ       = pset.get<double>("NoiseWidthZ");
  fLowCutoffZ        = pset.get<double>("LowCutoffZ");
  fNoiseFactU        = pset.get<double>("NoiseFactU");
  fNoiseWidthU       = pset.get<double>("NoiseWidthU");
  fLowCutoffU        = pset.get<double>("LowCutoffU");
  fNoiseFactV        = pset.get<double>("NoiseFactV");
  fNoiseWidthV       = pset.get<double>("NoiseWidthV");
  fLowCutoffV        = pset.get<double>("LowCutoffV");
  fNoiseArrayPoints  = pset.get<unsigned int>("NoiseArrayPoints");
  fOldNoiseIndex     = pset.get<bool>("OldNoiseIndex");
  fNoiseZ.resize(fNoiseArrayPoints);
  fNoiseU.resize(fNoiseArrayPoints);
  fNoiseV.resize(fNoiseArrayPoints);
  art::ServiceHandle<artext::SeedService> seedSvc;
#ifdef UseSeedService
  int seed = seedSvc->getSeed("ExponentialChannelNoiseService");
#else
  int seed = 1005;
#endif
  art::ServiceHandle<art::TFileService> tfs;
  fNoiseHist     = tfs->make<TH1F>("Noise", ";Noise  (ADC);", 1000,   -10., 10.);
  fNoiseChanHist = tfs->make<TH1F>("NoiseChan", ";Noise channel;", fNoiseArrayPoints, 0, fNoiseArrayPoints);
  art::EngineCreator ecr;
  m_pran = &ecr.createEngine(seed, "HepJamesRandom", "ExponentialChannelNoiseService");
  for ( unsigned int isam=0; isam<fNoiseArrayPoints; ++isam ) {
    generateNoise(fNoiseFactZ, fNoiseWidthZ, fLowCutoffZ, fNoiseZ[isam]);
    generateNoise(fNoiseFactU, fNoiseWidthU, fLowCutoffU, fNoiseU[isam]);
    generateNoise(fNoiseFactV, fNoiseWidthV, fLowCutoffV, fNoiseV[isam]);
  }
}

//**********************************************************************

int ExponentialChannelNoiseService::addNoise(Channel chan, AdcSignalVector& sigs) const {
#ifdef UseSeedService
  art::ServiceHandle<art::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine& engine = rng->getEngine("ExponentialChannelNoiseServiceIndex");
#else
  CLHEP::HepRandomEngine& engine = *m_pran;
#endif
  CLHEP::RandFlat flat(engine);
  unsigned int noisechan = 0;
  if ( fOldNoiseIndex ) {
    // Keep this strange way of choosing noise channel to be consistent with old results.
    // The relative weights of the first and last channels are 0.5 and 0.6.
    noisechan = nearbyint(flat.fire()*(1.*(fNoiseArrayPoints-1)+0.1));
  } else {
    noisechan = flat.fire()*fNoiseArrayPoints;
    if ( noisechan == fNoiseArrayPoints ) --noisechan;
  }
  fNoiseChanHist->Fill(noisechan);
  art::ServiceHandle<geo::Geometry> geo;
  const geo::View_t view = geo->View(chan);
  for ( unsigned int itck=0; itck<sigs.size(); ++itck ) {
    double tnoise = 0.0;
    if      ( view==geo::kU ) tnoise = fNoiseU[noisechan][itck];
    else if ( view==geo::kV ) tnoise = fNoiseV[noisechan][itck];
    else                      tnoise = fNoiseZ[noisechan][itck];
    sigs[itck] += tnoise;
  }
  return 0;
}

//**********************************************************************

ostream& ExponentialChannelNoiseService::print(ostream& out, string prefix) const {
  string myprefix = prefix + "  ";
  out << myprefix << "       NoiseFactZ: " << fNoiseFactZ   << endl;
  out << myprefix << "      NoiseWidthZ: " << fNoiseWidthZ  << endl;
  out << myprefix << "       LowCutoffZ: " << fLowCutoffZ << endl;
  out << myprefix << "       NoiseFactU: " << fNoiseFactU   << endl;
  out << myprefix << "      NoiseWidthU: " << fNoiseWidthU  << endl;
  out << myprefix << "       LowCutoffU: " << fLowCutoffU << endl;
  out << myprefix << "       NoiseFactV: " << fNoiseFactV   << endl;
  out << myprefix << "      NoiseWidthV: " << fNoiseWidthV  << endl;
  out << myprefix << "       LowCutoffV: " << fLowCutoffV << endl;
  out << myprefix << " NoiseArrayPoints: " << fNoiseArrayPoints << endl;
  out << myprefix << "    OldNoiseIndex: " << fOldNoiseIndex << endl;
  out << myprefix << "      Random seed: " << m_pran->getSeed() << endl;
  return out;
}

//**********************************************************************

void ExponentialChannelNoiseService::
generateNoise(float aNoiseFact, float aNoiseWidth, float aLowCutoff, AdcSignalVector& noise) const {
  // Fetch sampling rate.
  art::ServiceHandle<util::DetectorProperties> detprop;
  float sampleRate = detprop->SamplingRate();
  // Fetch FFT service and # ticks.
  art::ServiceHandle<util::LArFFT> pfft;
  unsigned int ntick = pfft->FFTSize();
  // Fetch random number engine.
#ifdef UseSeedService
  art::ServiceHandle<art::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine& engine = rng->getEngine("ExponentialChannelNoiseService");
#else
  CLHEP::HepRandomEngine& engine = *m_pran;
#endif
  CLHEP::RandFlat flat(engine);
  // Create noise spectrum in frequency.
  unsigned nbin = ntick/2 + 1;
  std::vector<TComplex> noiseFrequency(nbin, 0.);
  double pval = 0.;
  double lofilter = 0.;
  double phase = 0.;
  double rnd[2] = {0.};
  // width of frequencyBin in kHz
  double binWidth = 1.0/(ntick*sampleRate*1.0e-6);
  for ( unsigned int i=0; i<ntick/2+1; ++i ) {
    // exponential noise spectrum 
    pval = aNoiseFact*exp(-(double)i*binWidth/aNoiseWidth);
    // low frequency cutoff     
    lofilter = 1.0/(1.0+exp(-(i-aLowCutoff/binWidth)/0.5));
    // randomize 10%
    flat.fireArray(2, rnd, 0, 1);
    pval *= lofilter*(0.9 + 0.2*rnd[0]);
    // random pahse angle
    phase = rnd[1]*2.*TMath::Pi();
    TComplex tc(pval*cos(phase),pval*sin(phase));
    noiseFrequency[i] += tc;
  }
  // Obtain time spectrum from frequency spectrum.
  noise.clear();
  noise.resize(ntick,0.0);
  std::vector<double> tmpnoise(noise.size());
  pfft->DoInvFFT(noiseFrequency, tmpnoise);
  noiseFrequency.clear();
  // Multiply each noise value by ntick as the InvFFT 
  // divides each bin by ntick assuming that a forward FFT
  // has already been done.
  for ( unsigned int itck=0; itck<noise.size(); ++itck ) {
    noise[itck] = ntick*tmpnoise[itck];
  }
  for ( unsigned int itck=0; itck<noise.size(); ++itck ) {
    fNoiseHist->Fill(noise[itck]);
  }
}

//**********************************************************************

void ExponentialChannelNoiseService::generateNoise() {
  fNoiseZ.resize(fNoiseArrayPoints);
  fNoiseU.resize(fNoiseArrayPoints);
  fNoiseV.resize(fNoiseArrayPoints);
  for ( unsigned int inch=0; inch<fNoiseArrayPoints; ++inch ) {
    generateNoise(fNoiseFactZ, fNoiseWidthZ, fLowCutoffZ, fNoiseZ[inch]);
    generateNoise(fNoiseFactU, fNoiseWidthU, fLowCutoffU, fNoiseZ[inch]);
    generateNoise(fNoiseFactV, fNoiseWidthV, fLowCutoffV, fNoiseZ[inch]);
  }
}

//**********************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(ExponentialChannelNoiseService, ChannelNoiseService)

//**********************************************************************