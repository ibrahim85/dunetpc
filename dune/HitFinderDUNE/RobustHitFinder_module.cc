////////////////////////////////////////////////////////////////////////
// Class:       RobustHitFinder
// Module Type: producer
// File:        RobustHitFinder_module.cc
//
// Generated at Thu Aug 18 04:21:10 2016 by Matthew Thiesse using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

/*************************************************

October 2016

m.thiesse@sheffield.ac.uk

**************************************************/


#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "nutools/RandomUtils/NuRandomService.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/ExternalTrigger.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "dune/RunHistory/DetPedestalDUNE.h"

#include <math.h>
#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "TMath.h"
#include "TVector3.h"
#include "TTree.h"
#include "TF1.h"

#include "CLHEP/Random/RandomEngine.h"

//for the counter position map
#include "dune/daqinput35t/PennToOffline.h"

#include "RobustHitFinderSupport.h"
#include "HitLineFitAlg.h"
#include "RMSHitFinderAlg.h"

namespace dune {
  class RobustHitFinder;
}

class dune::RobustHitFinder : public art::EDProducer {
public:
  explicit RobustHitFinder(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  RobustHitFinder(RobustHitFinder const &) = delete;
  RobustHitFinder(RobustHitFinder &&) = delete;
  RobustHitFinder & operator = (RobustHitFinder const &) = delete;
  RobustHitFinder & operator = (RobustHitFinder &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

  // Selected optional functions.
  void beginJob() override;
  void reconfigure(fhicl::ParameterSet const & p) override;

private:
  void SetTreeVariables(const dune::ChannelInformation & chan, const dune::HitInformation & hit, const dune::HitLineFitAlg::HitLineFitResults & fitresult);
  void MakeupMissedHits(ChanMap_t & chanMap, HitVec_t & hitVec);
  void FillHitInformation(dune::ChannelInformation & chan, dune::HitVec_t & hitVec, bool assumedHit);
  bool ValidTrigger(std::vector<unsigned int> evtTriggers, unsigned int & c1arg, unsigned int & c2arg, unsigned int & trignumarg);
  float TimeToDriftDist(float thistime, unsigned int thistpc);
  float TimeToDisplacement(float thistime);
  float TimeToX(float thistime, unsigned int thistpc);
  float hitGeomDist(TVector3 hitloc, TVector3 trigloc1, TVector3 trigloc2);
  void Reset();


  TTree * fTree;
  int run;
  int event;
  double t0;
  unsigned int c1;
  unsigned int c2;
  unsigned int trignum;
  float c1x;
  float c1y;
  float c1z;
  float c2x;
  float c2y;
  float c2z;
  float distancecut;
  int channel;
  int wire;
  int tpc;
  int signalsize;
  std::vector<float> signal;
  std::vector<float> signalFilter;
  float baseline;
  float rms;
  float baselineFilter;
  float rmsFilter;
  float pedmean;
  float pedrms;
  float integral;
  float integralFilter;
  float sumADC;
  float sigmaintegral;
  float sigmaintegralFilter;
  float amplitude;
  float amplitudeFilter;
  float peaktick;
  float peaktickFilter;
  float peaktime;
  float peaktimeFilter;
  int begintick;
  int endtick;
  int width;
  float hitx;
  float hity;
  float hitz;
  float hiterrxlo;
  float hiterrxhi;
  float hiterrylo;
  float hiterryhi;
  float hiterrzlo;
  float hiterrzhi;
  float perpdist;
  float hitt;
  float driftdist;
  bool countercut;
  float fitconstant;
  float fitconstanterr;
  float fitlinear;
  float fitlinearerr;
  float fitquadratic;
  float fitquadraticerr;
  float fitchi2;
  float fitsumsqrresidual;
  float fitndf;
  float fitmle;
  bool fitsuccess;
  bool fitrealhit;
  float segmentlength;
  bool assumedhit;
  int numGoodHitsChan;
  int nwiresTPC0;
  int nwiresTPC1;
  int nwiresTPC2;
  int nwiresTPC3;
  int nwiresTPC4;
  int nwiresTPC5;
  int nwiresTPC6;
  int nwiresTPC7;

  bool fMakeTree;
  float fHitGeomDistanceCut;
  std::string fWireModuleLabel;
  std::string fCounterT0ModuleLabel;
  std::vector<float> fEfield;
  int fSearchPreTicks;
  int fSearchPostTicks;
  bool fMakeupMissedHits;
  int fMissedBufferTicksLow;
  int fMissedBufferTicksHigh;
  float fMinPedMean;
  float fMaxPedMean;
  float fMinPedRms;
  float fMaxPedRms;

  float fHorizRangeMin;
  float fHorizRangeMax;
  float fVertRangeMin;
  float fVertRangeMax;
  float fC1Vert;
  float fC1Horiz;
  float fC2Vert;
  float fC2Horiz;
  UInt_t fSeedValue;
  std::map<unsigned int, std::pair<TVector3, std::vector<TVector3> > > fCounterPositionMap;

  dune::HitLineFitAlg fFitAlg;
  dune::RMSHitFinderAlg fHitFinderAlg;

  art::ServiceHandle<geo::Geometry> fGeom;
  art::ServiceHandle<art::RandomNumberGenerator> fRng;
  art::ServiceHandle<rndm::NuRandomService> fSeed;
  art::ServiceHandle<art::TFileService> fTfs;
  detinfo::DetectorClocks const * fClks;
  detinfo::DetectorProperties const * fDetProp;
  const lariov::DetPedestalProvider& fPedestalRetrievalAlg = *(lar::providerFrom<lariov::DetPedestalService>());
};


dune::RobustHitFinder::RobustHitFinder(fhicl::ParameterSet const & p)
  : fFitAlg(p.get<fhicl::ParameterSet>("HitLineFitAlg")),
    fHitFinderAlg(p.get<fhicl::ParameterSet>("RMSHitFinderAlg")),
    fClks(lar::providerFrom<detinfo::DetectorClocksService>()),
    fDetProp(lar::providerFrom<detinfo::DetectorPropertiesService>())

{
  this->reconfigure(p);

  fSeed->createEngine(*this,"HepJamesRandom","Seed");

  recob::HitCollectionCreator::declare_products(*this);
}

void dune::RobustHitFinder::produce(art::Event & e)
{
  Reset();

  recob::HitCollectionCreator hcol(*this, e);

  CLHEP::HepRandomEngine const & engine = fRng->getEngine("Seed");
  fFitAlg.SetSeed(engine.getSeed());

  // get recob::Wires
  art::Handle< std::vector< recob::Wire> > wireHandle;
  if (!e.getByLabel(fWireModuleLabel,wireHandle))
    {
      mf::LogError("RobustHitFinder") << "No recob::Wires found when expected";
      hcol.put_into(e);
      return;
    }

  // get associated raw digits
  art::FindOneP<raw::RawDigit> rawdigits(wireHandle,e,fWireModuleLabel);

  // Get all T0 products
  art::Handle< std::vector< anab::T0> > t0Handle;
  if (!e.getByLabel(fCounterT0ModuleLabel,t0Handle))
    {
      mf::LogError("RobustHitFinder") << "No T0s found in this event";
      hcol.put_into(e);
      return;
    }

  // get associated external triggers
  art::FindManyP<raw::ExternalTrigger> triggers(t0Handle,e,fCounterT0ModuleLabel);

  run = e.run();
  event = e.event();

  
  for (size_t iwire = 0; iwire < wireHandle->size(); ++iwire)
    {
      art::Ptr<recob::Wire> wire(wireHandle,iwire);
      if (wire->View() != geo::kZ) continue;  
      tpc = fGeom->ChannelToWire(wire->Channel())[0].TPC;
      if (tpc==0) nwiresTPC0++;
      else if (tpc==1) nwiresTPC1++;
      else if (tpc==2) nwiresTPC2++;
      else if (tpc==3) nwiresTPC3++;
      else if (tpc==4) nwiresTPC4++;
      else if (tpc==5) nwiresTPC5++;
      else if (tpc==6) nwiresTPC6++;
      else if (tpc==7) nwiresTPC7++;
      else mf::LogVerbatim("RobustHitFinder") << "Unknown TPC";
    }

  for (size_t i_t0 = 0; i_t0 < t0Handle->size(); i_t0++)
    {
      art::Ptr<anab::T0> pt0(t0Handle,i_t0);
      t0 = pt0->Time();

      unsigned int tick0 = static_cast<unsigned int>(fClks->TPCG4Time2Tick(t0));
      mf::LogVerbatim("RobustHitFinder") << "Found t0 at TPC tick = " << tick0;

      std::vector<art::Ptr<raw::ExternalTrigger> > trigvec = triggers.at(i_t0);

      std::vector<unsigned int> evtTriggers;
      for (auto const &trig : trigvec) evtTriggers.push_back(trig->GetTrigID());

      if (!ValidTrigger(evtTriggers,c1,c2,trignum)) continue;

      c1x = fCounterPositionMap[c1].first.X();
      c1y = fCounterPositionMap[c1].first.Y();
      c1z = fCounterPositionMap[c1].first.Z();
      c2x = fCounterPositionMap[c2].first.X();
      c2y = fCounterPositionMap[c2].first.Y();
      c2z = fCounterPositionMap[c2].first.Z();
    
      distancecut = fHitGeomDistanceCut;
      if (trignum == 111)
        {
          fHorizRangeMin = -10;
          fHorizRangeMax = 170;
          fVertRangeMin = -50;
          fVertRangeMax = 250;
          fC1Vert = c1x;
          fC1Horiz = c1z;
          fC2Vert = c2x;
          fC2Horiz = c2z;
          distancecut *= fabs(TMath::Sin(TMath::ATan2(c1z-c2z,c1x-c2x)));
        }
      if (trignum == 112 || trignum == 113)
        {
          fHorizRangeMin = -50;
          fHorizRangeMax = 250;
          fVertRangeMin = -10;
          fVertRangeMax = 170;
          fC1Vert = c1z;
          fC1Horiz = c1x;
          fC2Vert = c2z;
          fC2Horiz = c2x;
          distancecut *= fabs(TMath::Cos(TMath::ATan2(c1z-c2z,c1x-c2x)));
        }
      double counterslope = ((fC1Vert-fC2Vert)/(fC1Horiz-fC2Horiz));
      fFitAlg.SetParameter(0,10,fVertRangeMin,fVertRangeMax);
      fFitAlg.SetParameter(1,1,counterslope-0.15,counterslope+0.15);
      fFitAlg.SetParameter(2,0,-0.0002,0.0002);
      fFitAlg.SetHorizVertRanges(fHorizRangeMin,fHorizRangeMax,fVertRangeMin,fVertRangeMax);

      dune::ChanMap_t chanMap;

      for (size_t iwire = 0; iwire < wireHandle->size(); ++iwire)
	{
	  art::Ptr<recob::Wire> pwire(wireHandle,iwire);
	  art::Ptr<raw::RawDigit> prawdigit = rawdigits.at(iwire);
	  
	  if (pwire->View() != geo::kZ) continue;
	  
	  dune::ChannelInformation chan;
	  
	  chan.artWire = pwire;
	  chan.artRawDigit = prawdigit;
	  
	  chan.signalVec.clear();
	  chan.signalFilterVec.clear();
	  
	  chan.channelID = pwire->Channel();
	  chan.wireID = fGeom->ChannelToWire(chan.channelID)[0].Wire;
	  chan.tpcNum = fGeom->ChannelToWire(chan.channelID)[0].TPC;
	  
	  chan.signalVec = pwire->Signal();
	  chan.signalSize = chan.signalVec.size();

	  double wirexyz[3];
          fGeom->Wire(*(fGeom->ChannelToWire(chan.channelID).begin())).GetCenter(wirexyz);
	  chan.chanz = static_cast<float>(wirexyz[2]);
	  
	  fHitFinderAlg.SetSearchTicks(tick0 - ((fSearchPreTicks>0) ? fSearchPreTicks : 0),
				       tick0 + ((fSearchPostTicks>0) ? fSearchPostTicks : chan.signalSize));
	  fHitFinderAlg.FilterWaveform(chan.signalVec,chan.signalFilterVec);
	  fHitFinderAlg.RobustRMSBase(chan.signalVec,chan.baseline,chan.rms);
	  fHitFinderAlg.RobustRMSBase(chan.signalFilterVec,chan.baselineFilter,chan.rmsFilter);

	  pedmean = fPedestalRetrievalAlg.PedMean(chan.channelID);
	  pedrms = fPedestalRetrievalAlg.PedRms(chan.channelID);

	  if (pedmean > fMaxPedMean || pedmean < fMinPedMean || pedrms > fMaxPedRms || pedrms < fMinPedRms) continue;

	  fHitFinderAlg.FindHits(chan);

	  chanMap.emplace(std::make_pair(chan.channelID,chan));
	}

      dune::HitVec_t hitVec;
      for (auto & chanmapitr : chanMap)
	{
	  FillHitInformation(chanmapitr.second,hitVec,false);
	}
      
      std::vector<dune::HitLineFitAlg::HitLineFitData> fitdata;
      for (auto & hit : hitVec)
	{
	  dune::HitLineFitAlg::HitLineFitData hlfd;
	  if (trignum == 111)
	    {
	      hlfd.hitHoriz = hit.hitz;
	      hlfd.hitVert = hit.hitx;
	      hlfd.hitHorizErrLo = hit.hiterrzlo;
	      hlfd.hitHorizErrHi = hit.hiterrzhi;
	      hlfd.hitVertErrLo = hit.hiterrxlo;
	      hlfd.hitVertErrHi = hit.hiterrxhi;
	    }
	  else if (trignum == 112 || trignum == 113)
	    {
	      hlfd.hitHoriz = hit.hitx;
	      hlfd.hitVert = hit.hitz;
	      hlfd.hitHorizErrLo = hit.hiterrxlo;
	      hlfd.hitHorizErrHi = hit.hiterrxhi;
	      hlfd.hitVertErrLo = hit.hiterrzlo;
	      hlfd.hitVertErrHi = hit.hiterrzhi;
	    }
	  hlfd.hitREAL = false;
	  fitdata.push_back(hlfd);
	}

      dune::HitLineFitAlg::HitLineFitResults fitresult;
      int retval = fFitAlg.FitLine(fitdata,fitresult);

      for (size_t i_h = 0; i_h < fitdata.size(); ++i_h)
	{
	  hitVec[i_h].fitrealhit = fitdata[i_h].hitREAL;
	}

      if (fMakeupMissedHits && retval == 1) MakeupMissedHits(chanMap,hitVec);
      
      if (retval == 1 || retval == 0)
	{
	  for (auto & hit : hitVec)
	    {
	      //dune::HitInformation & hit = hitVec.at(i_hit);//!!!!!!!!!
	      dune::ChannelInformation & chan = chanMap[hit.channelID];
	      
	      SetTreeVariables(chan,hit,fitresult);
	      
	      if (fMakeTree) fTree->Fill();
	      if (hit.fitrealhit)
		{
		  hcol.emplace_back(hit.artHit,chan.artWire,chan.artRawDigit);
		}
	    }
	}
    }
  hcol.put_into(e);
}

void dune::RobustHitFinder::beginJob()
{
  DAQToOffline::MakeCounterPositionMap("","counterInformation.txt",fCounterPositionMap,0,0,0);

  if (fMakeTree)
    {
      fTree = fTfs->make<TTree>("RobustHitFinder","RobustHitFinder");
      fTree->Branch("run",&run,"run/I");
      fTree->Branch("event",&event,"event/I");
      fTree->Branch("t0",&t0,"t0/D");
      fTree->Branch("c1",&c1,"c1/i");
      fTree->Branch("c2",&c2,"c2/i");
      fTree->Branch("trignum",&trignum,"trignum/i");
      fTree->Branch("c1x",&c1x,"c1x/F");
      fTree->Branch("c1y",&c1y,"c1y/F");
      fTree->Branch("c1z",&c1z,"c1z/F");
      fTree->Branch("c2x",&c2x,"c2x/F");
      fTree->Branch("c2y",&c2y,"c2y/F");
      fTree->Branch("c2z",&c2z,"c2z/F");
      fTree->Branch("distancecut",&distancecut,"distancecut/F");
      fTree->Branch("channel",&channel,"channel/I");
      fTree->Branch("wire",&wire,"wire/I");
      fTree->Branch("tpc",&tpc,"tpc/I");
      fTree->Branch("signalsize",&signalsize,"signalsize/I");
      //fTree->Branch("signal",&signal);
      //fTree->Branch("signalFilter",&signalFilter);
      fTree->Branch("baseline",&baseline,"baseline/F");
      fTree->Branch("rms",&rms,"rms/F");
      fTree->Branch("baselineFilter",&baselineFilter,"baselineFilter/F");
      fTree->Branch("rmsFilter",&rmsFilter,"rmsFilter/F");
      fTree->Branch("pedmean",&pedmean,"pedmean/F");
      fTree->Branch("pedrms",&pedrms,"pedrms/F");
      fTree->Branch("integral",&integral,"integral/F");
      fTree->Branch("integralFilter",&integralFilter,"integralFilter/F");
      fTree->Branch("sumADC",&sumADC,"sumADC/F");
      fTree->Branch("sigmaintegral",&sigmaintegral,"sigmaintegral/F");
      fTree->Branch("sigmaintegralFilter",&sigmaintegralFilter,"sigmaintegralFilter/F");
      fTree->Branch("amplitude",&amplitude,"amplitude/F");
      fTree->Branch("amplitudeFilter",&amplitudeFilter,"amplitudeFilter/F");
      fTree->Branch("peaktick",&peaktick,"peaktick/F");
      fTree->Branch("peaktickFilter",&peaktickFilter,"peaktickFilter/F");
      fTree->Branch("peaktime",&peaktime,"peaktime/F");
      fTree->Branch("peaktimeFilter",&peaktimeFilter,"peaktimeFilter/F");
      fTree->Branch("begintick",&begintick,"begintick/I");
      fTree->Branch("endtick",&endtick,"endtick/I");
      fTree->Branch("width",&width,"width/I");
      fTree->Branch("hitx",&hitx,"hitx/F");
      fTree->Branch("hity",&hity,"hity/F");
      fTree->Branch("hitz",&hitz,"hitz/F");
      fTree->Branch("hiterrxlo",&hiterrxlo,"hiterrxlo/F");
      fTree->Branch("hiterrxhi",&hiterrxhi,"hiterrxhi/F");
      fTree->Branch("hiterrylo",&hiterrylo,"hiterrylo/F");
      fTree->Branch("hiterryhi",&hiterryhi,"hiterryhi/F");
      fTree->Branch("hiterrzlo",&hiterrzlo,"hiterrzlo/F");
      fTree->Branch("hiterrzhi",&hiterrzhi,"hiterrzhi/F");
      fTree->Branch("perpdist",&perpdist,"perpdist/F");
      fTree->Branch("hitt",&hitt,"hitt/F");
      fTree->Branch("driftdist",&driftdist,"driftdist/F");
      fTree->Branch("countercut",&countercut,"countercut/O");
      fTree->Branch("fitconstant",&fitconstant,"fitconstant/F");
      fTree->Branch("fitconstanterr",&fitconstanterr,"fitconstanterr/F");
      fTree->Branch("fitlinear",&fitlinear,"fitlinear/F");
      fTree->Branch("fitlinearerr",&fitlinearerr,"fitlinearerr/F");
      fTree->Branch("fitquadratic",&fitquadratic,"fitquadratic/F");
      fTree->Branch("fitquadraticerr",&fitquadraticerr,"fitquadraticerr/F");
      fTree->Branch("fitchi2",&fitchi2,"fitchi2/F");
      fTree->Branch("fitsumsqrresidual",&fitsumsqrresidual,"fitsumsqrresidual/F");
      fTree->Branch("fitndf",&fitndf,"fitndf/F");
      fTree->Branch("fitmle",&fitmle,"fitmle/F");
      fTree->Branch("fitsuccess",&fitsuccess,"fitsuccess/O");
      fTree->Branch("fitrealhit",&fitrealhit,"fitrealhit/O");
      fTree->Branch("assumedhit",&assumedhit,"assumedhit/O");
      fTree->Branch("segmentlength",&segmentlength,"segmentlength/F");
      fTree->Branch("numGoodHitsChan",&numGoodHitsChan,"numGoodHitsChan/I");
      fTree->Branch("nwiresTPC0",&nwiresTPC0,"nwiresTPC0/I");
      fTree->Branch("nwiresTPC1",&nwiresTPC1,"nwiresTPC1/I");
      fTree->Branch("nwiresTPC2",&nwiresTPC2,"nwiresTPC2/I");
      fTree->Branch("nwiresTPC3",&nwiresTPC3,"nwiresTPC3/I");
      fTree->Branch("nwiresTPC4",&nwiresTPC4,"nwiresTPC4/I");
      fTree->Branch("nwiresTPC5",&nwiresTPC5,"nwiresTPC5/I");
      fTree->Branch("nwiresTPC6",&nwiresTPC6,"nwiresTPC6/I");
      fTree->Branch("nwiresTPC7",&nwiresTPC7,"nwiresTPC7/I");
    }
}

void dune::RobustHitFinder::reconfigure(fhicl::ParameterSet const & p)
{
  fMakeTree = p.get<bool>("MakeTree");
  fHitGeomDistanceCut = p.get<float>("HitGeomDistanceCut");
  fCounterT0ModuleLabel = p.get<std::string>("CounterT0ModuleLabel");
  fWireModuleLabel = p.get<std::string>("WireModuleLabel");
  fEfield = p.get<std::vector<float> >("Efield");
  fSearchPreTicks = p.get<int>("SearchPreTicks",-1);
  fSearchPostTicks = p.get<int>("SearchPostTicks",-1);
  fMinPedMean = p.get<float>("MinPedMean");
  fMaxPedMean = p.get<float>("MaxPedMean");
  fMinPedRms = p.get<float>("MinPedRms");
  fMaxPedRms = p.get<float>("MaxPedRms");
  fMakeupMissedHits = p.get<bool>("MakeupMissedHits",true);
  fMissedBufferTicksLow = p.get<int>("MissedBufferTicksLow");
  fMissedBufferTicksHigh = p.get<int>("MissedBufferTicksHigh");
}

void dune::RobustHitFinder::MakeupMissedHits(ChanMap_t & chanMap, HitVec_t & hitVec)
{
  for (int i_tpc = 0; i_tpc<8; ++i_tpc)
    {
      float chanbeginz = 99999;
      float chanendz = -99999;
      int chanbeginid = 99999;
      int chanendid = -99999;
      for (auto & chanitr : chanMap)
	{
	  if (chanitr.second.tpcNum != i_tpc) continue;
	  chanitr.second.nGoodHits = 0;
	  for (auto & hit : hitVec)
	    {
	      if (hit.channelID == chanitr.first && hit.fitrealhit)
		{
		  chanitr.second.goodHitStartTick = hit.hitBeginTick;
		  chanitr.second.goodHitEndTick = hit.hitEndTick;
		  chanitr.second.nGoodHits++;
		}
	    }
	  if (chanitr.second.nGoodHits > 0 && chanitr.second.chanz < chanbeginz)
	    {
	      chanbeginz = chanitr.second.chanz;
	      chanbeginid = chanitr.first;
	    }
	  if (chanitr.second.nGoodHits > 0 && chanitr.second.chanz > chanendz)
	    {
	      chanendz = chanitr.second.chanz;
	      chanendid = chanitr.first;
	    }
	}
      
      if (chanendz < chanbeginz)
	{
	  mf::LogError("RobustHitFinder") << "Problem: chanbeginz=" << chanbeginz << " is greater than chanendz=" << chanendz;
	  return;
	}
      
      if (chanbeginid == 99999 || chanendid == -99999)
	{
	  mf::LogError("RobustHitFinder") << "Track not found. No assumed hits added." << std::endl;
	  return;
	}
      
      if (chanMap[chanbeginid].tpcNum % 2 != chanMap[chanendid].tpcNum % 2)
	{
	  mf::LogError("RobustHitFinder") << "Track crosses APA. Don't know how to deal with this yet.";
	  return;
	}
      
      std::vector<std::pair<float,int> > trackchans;
      for (auto const & chanitr : chanMap)
	{
	  if (chanitr.second.chanz >= chanbeginz && chanitr.second.chanz <= chanendz && chanitr.second.tpcNum % 2 == chanMap[chanbeginid].tpcNum % 2)
	    {
	      trackchans.push_back(std::make_pair(chanitr.second.chanz,chanitr.first));
	    }
	}
      std::sort(trackchans.begin(),trackchans.end());
      
      for (size_t i_tc = 0; i_tc < trackchans.size(); ++i_tc)
	{
	  dune::ChannelInformation ch = chanMap[trackchans[i_tc].second];
	  
	  if (ch.nGoodHits == 0 && ch.chanz >= chanbeginz && ch.chanz <= chanendz)
	    {
	      int sub = 0;
	      dune::ChannelInformation chnearlow = ch;
	      while (chnearlow.nGoodHits != 1 && chnearlow.chanz >= chanbeginz)
		{
		  ++sub;
		  chnearlow = chanMap[trackchans[i_tc-sub].second];
		}
	      
	      int add = 0;
	      dune::ChannelInformation chnearhigh = ch;
	      while (chnearhigh.nGoodHits != 1 && chnearhigh.chanz <= chanendz)
		{
		  ++add;
		  chnearhigh = chanMap[trackchans[i_tc+add].second];
		}
	      
	      if (ch.tpcNum != chnearlow.tpcNum || ch.tpcNum != chnearhigh.tpcNum) continue;
	      
	      int cls = chnearlow.goodHitStartTick;
	      int chs = chnearhigh.goodHitStartTick;
	      int cle = chnearlow.goodHitEndTick;
	      int che = chnearhigh.goodHitEndTick;
	      double clz = chnearlow.chanz;
	      double chz = chnearhigh.chanz;
	      
	      int chstart = cls + (((chs - cls) / (chz - clz)) * (ch.chanz - clz));
	      int chend   = cle + (((che - cle) / (chz - clz)) * (ch.chanz - clz));
	      
	      chstart -= fMissedBufferTicksLow;
	      chend   += fMissedBufferTicksHigh;
	      
	      if (chz-clz < 0) continue; // something really weird must've happened
	      
	      ch.pulse_ends.push_back(std::make_pair(chstart,chend));
	      
	      FillHitInformation(ch,hitVec,true);
	    }
	}
    }
}

void dune::RobustHitFinder::SetTreeVariables(const dune::ChannelInformation & chan, const dune::HitInformation & hit, const dune::HitLineFitAlg::HitLineFitResults & fitresult)
{
  fitsuccess = fitresult.fitsuccess;
  TF1 * model = new TF1("model","pol2",fHorizRangeMin,fHorizRangeMax);
  if (fitsuccess)
    {
      fitconstant = fitresult.bestVal.at(0);
      fitconstanterr = fitresult.bestValError.at(0);
      fitlinear = fitresult.bestVal.at(1);
      fitlinearerr = fitresult.bestValError.at(1);
      fitquadratic = fitresult.bestVal.at(2);
      fitquadraticerr = fitresult.bestValError.at(2);
      fitchi2 = fitresult.chi2;
      fitsumsqrresidual = fitresult.sum2resid;
      fitmle = fitresult.mle;
      fitndf = fitresult.ndf;
      model->SetParameters(fitconstant,fitlinear,fitquadratic);
    }

  channel = chan.channelID;
  wire = chan.wireID;
  tpc = chan.tpcNum;
  signalsize = chan.signalSize;
  signal = chan.signalVec;
  signalFilter = chan.signalFilterVec;
  baseline = chan.baseline;
  rms = chan.rms;
  baselineFilter = chan.baselineFilter;
  rmsFilter = chan.rmsFilter;
  integral = hit.hitIntegral;
  integralFilter = hit.hitIntegralFilter;
  sumADC = hit.hitSumADC;
  sigmaintegral = hit.hitSigmaIntegral;
  sigmaintegralFilter = hit.hitSigmaIntegralFilter;
  amplitude = hit.hitAmplitude;
  amplitudeFilter = hit.hitAmplitudeFilter;
  peaktick = hit.hitPeakTick;
  peaktickFilter = hit.hitPeakTickFilter;
  peaktime = hit.hitPeakTime;
  peaktimeFilter = hit.hitPeakTimeFilter;
  begintick = hit.hitBeginTick;
  endtick = hit.hitEndTick;
  width = hit.hitWidth;
  hitx = hit.hitx;
  hity = hit.hity;
  hitz = hit.hitz;
  hiterrxlo = hit.hiterrxlo;
  hiterrxhi = hit.hiterrxhi;
  hiterrylo = hit.hiterrylo;
  hiterryhi = hit.hiterryhi;
  hiterrzlo = hit.hiterrzlo;
  hiterrzhi = hit.hiterrzhi;
  perpdist = hit.perpdist;
  hitt = hit.hitt;
  driftdist = hit.driftdist;
  countercut = hit.countercut;
  fitrealhit = hit.fitrealhit;
  assumedhit = hit.assumedhit;
  numGoodHitsChan = chan.nGoodHits;
  
  segmentlength = 0.449;
  if (fitsuccess && fitrealhit)
    {
      if (trignum == 111)
	{
	  double thetayz = TMath::ATan2(model->Eval(hitz+1)-model->Eval(hitz-1),2);
	  double tan2thetayz = TMath::Power(TMath::Tan(thetayz),2);
	  double y2z2 = ((c1y-c2y)*(c1y-c2y))/((c1z-c2z)*(c1z-c2z));
	  double projL = sqrt(1+tan2thetayz+y2z2);
	  segmentlength *= static_cast<float>(projL);
	}
      else if (trignum == 112 || trignum == 113)
	{
	  double thetayx = TMath::ATan2(2,model->Eval(hitx+1)-model->Eval(hitx-1));
	  double tan2thetayx = TMath::Power(TMath::Tan(thetayx),2);
	  double y2x2 = ((c1y-c2y)*(c1y-c2y))/((c1x-c2x)*(c1x-c2x));
	  double projL = sqrt(1+tan2thetayx*(1+y2x2));
	  segmentlength *= static_cast<float>(projL);
	}
    }
}

void dune::RobustHitFinder::FillHitInformation(dune::ChannelInformation & chan, dune::HitVec_t & hitVec, bool assumedHit)
{
  TF1 * gaus = new TF1("gaus","([0]/([2]*sqrt(2*3.1415926)))*exp(-0.5*(x-[1])*(x-[1])/([2]*[2]))+[3]+x*[4]",0,32000);
  gaus->SetNpx(64000);

  for (size_t i_hit = 0; i_hit < chan.pulse_ends.size(); i_hit++)
    {
      dune::HitInformation hit;
      hit.channelID = chan.channelID;
      int begin_index = chan.pulse_ends[i_hit].first;
      int end_index = chan.pulse_ends[i_hit].second;
      hit.hitBeginTick = begin_index;
      hit.hitEndTick = end_index;
      std::vector<float>::iterator beginitr = chan.signalVec.begin()+begin_index;
      std::vector<float>::iterator enditr = chan.signalVec.begin()+end_index;
      std::vector<float> pulse(beginitr,enditr);
      std::vector<float>::iterator fbeginitr = chan.signalFilterVec.begin()+begin_index;
      std::vector<float>::iterator fenditr = chan.signalFilterVec.begin()+end_index;
      std::vector<float> pulseFilter(fbeginitr,fenditr);
      hit.hitAmplitude = *std::max_element(beginitr,enditr)-chan.baseline;
      hit.hitAmplitudeFilter = *std::max_element(fbeginitr,fenditr)-chan.baselineFilter;
      hit.hitWidth = end_index-begin_index;
      hit.hitIntegral = std::accumulate(beginitr,enditr,0)-(hit.hitWidth*chan.baseline);
      hit.hitSumADC = hit.hitIntegral;
      hit.hitIntegralFilter = std::accumulate(fbeginitr,fenditr,0)-(hit.hitWidth*chan.baselineFilter);
      hit.hitSigmaIntegral = TMath::Sqrt(pulse.size())*TMath::RMS(pulse.size(),pulse.data());
      hit.hitSigmaIntegralFilter = TMath::Sqrt(pulseFilter.size())*TMath::RMS(pulseFilter.size(),pulseFilter.data());
      hit.hitPeakTick = std::distance(chan.signalVec.begin(),std::max_element(beginitr,enditr));
      hit.hitPeakTickFilter = std::distance(chan.signalFilterVec.begin(),std::max_element(fbeginitr,fenditr));
      hit.hitPeakTime = fClks->TPCTick2TrigTime(hit.hitPeakTick);
      hit.hitPeakTimeFilter = fClks->TPCTick2TrigTime(hit.hitPeakTickFilter);

      /*
      gaus->SetParameter(1,hit.hitPeakTick);
      gaus->SetParLimits(1,hit.hitPeakTick-10,hit.hitPeakTick+10);
      gaus->SetParameter(2,5);
      gaus->SetParLimits(2,1,end_index-begin_index);
      TGraph * gr = new TGraph();
      Int_t tick = 0;
      for (auto adc : chan.signalVec)
	{
	  gr->SetPoint(tick,(Double_t)tick,(Double_t)adc);
	  ++tick;
	}
      gr->Fit(gaus,"BQ0");

      hit.hitAmplitude = gaus->GetParameter(0)/(gaus->GetParameter(2)*sqrt(2*3.1415926));
      hit.hitIntegral = gaus->GetParameter(0);
      hit.hitSigmaIntegral = gaus->GetParError(0);
      hit.hitSumADC = std::accumulate(beginitr,enditr,0)-((end_index-begin_index)*chan.baseline);
      hit.hitWidth = gaus->GetParameter(2);
      hit.hitPeakTick = gaus->GetParameter(1);
      hit.hitPeakTime = fClks->TPCTick2TrigTime(hit.hitPeakTick);
      */

      hit.hitt = hit.hitPeakTime - t0/1000;

      hit.hitx = TimeToX(hit.hitt,chan.tpcNum);
      hit.hity = 2.;
      hit.hitz = chan.chanz;
      hit.hiterrxlo = TMath::Sqrt(fabs(hit.hitx-TimeToDisplacement(fClks->TPCTick2TrigTime(hit.hitBeginTick)-t0/1000)));
      hit.hiterrxhi = TMath::Sqrt(fabs(hit.hitx-TimeToDisplacement(fClks->TPCTick2TrigTime(hit.hitEndTick)-t0/1000)));
      hit.hiterrylo = 0.;
      hit.hiterryhi = 0.;
      hit.hiterrzlo = 0.5/sqrt(12.0);
      hit.hiterrzhi = 0.5/sqrt(12.0);
      hit.driftdist = TimeToDriftDist(hit.hitt,chan.tpcNum);
      hit.perpdist = hitGeomDist(TVector3(hit.hitx,2.,hit.hitz),TVector3(c1x,2.,c1z),TVector3(c2x,2.,c2z));
      hit.countercut = (hit.perpdist < distancecut) ? true : false;
      hit.fitrealhit = false;
      hit.assumedhit = assumedHit;

      recob::HitCreator temphit(*(chan.artWire),
				fGeom->ChannelToWire(chan.channelID)[0],
				hit.hitBeginTick,
				hit.hitEndTick,
				hit.hitWidth,
				hit.hitPeakTick,
				gaus->GetParError(1),
				hit.hitAmplitude,
				sqrt(hit.hitAmplitude),
				hit.hitIntegral,
				hit.hitSigmaIntegral,
				hit.hitSumADC,
				1,
				-1,
				gaus->GetChisquare(),
				gaus->GetNDF());
      hit.artHit = temphit.move();
      if (hit.hitx > -400 && hit.countercut) hitVec.push_back(hit);
      
      //delete gr;
    }
}

bool dune::RobustHitFinder::ValidTrigger(std::vector<unsigned int> evtTriggers, unsigned int & c1arg, unsigned int & c2arg, unsigned int & trignumarg)
{
  c1arg=999; c2arg=999;
  int contains_111 = 0, contains_112 = 0, contains_113 = 0;
  int contains_Ntrigs = 0, contains_NU = 0, contains_NL = 0, contains_SU = 0, contains_SL = 0;
  int contains_EL = 0, contains_WU = 0, contains_TEL = 0;
  for (size_t i_c = 0; i_c < evtTriggers.size(); i_c++)
    {
      unsigned int trigID = evtTriggers[i_c];
      if (trigID >= 0 && trigID <= 5) contains_SL++;
      if (trigID >= 6 && trigID <= 15) contains_EL++;
      if (trigID >= 16 && trigID <= 21) contains_NL++;
      if (trigID >= 22 && trigID <= 27) contains_NU++;
      if (trigID >= 28 && trigID <= 37) contains_WU++;
      if (trigID >= 38 && trigID <= 43) contains_SU++;
      if (trigID >= 44 && trigID <= 92) contains_TEL++;
      if (trigID == 111) contains_111++;
      if (trigID == 112) contains_112++;
      if (trigID == 113) contains_113++;
      contains_Ntrigs++;
    }
  if (contains_111 + contains_112 + contains_113 != 1) return false;        // too many/few coincidences!
  if (contains_TEL &&
      (contains_NU || contains_NL || contains_SU || contains_SL || contains_EL || contains_WU)) return false;        // track probably doesn't go through detector
  if (contains_Ntrigs != 3) return false;        // too much/little going on!
  if (contains_111 && (contains_NU || contains_NL || contains_SU || contains_SL)) return false;        // 111 should not have NU/NL/SU/SL
  if (contains_112 && (contains_EL || contains_WU || contains_SU || contains_NL)) return false;        // 112 should not have EL/WU/SU/NL
  if (contains_113 && (contains_EL || contains_WU || contains_NU || contains_SL)) return false;        // 113 should not have EL/WU/NU/SL
  if (contains_111 && (!contains_EL || !contains_WU)) return false;        // incomplete trigger
  if (contains_112 && (!contains_NU || !contains_SL)) return false;        // incomplete trigger
  if (contains_113 && (!contains_SU || !contains_NL)) return false;        // incomplete trigger

  std::vector<unsigned int> counterIDs;
  trignumarg = 0;
  for (size_t i_c = 0; i_c < evtTriggers.size(); i_c++)
    {
      unsigned int trigID = evtTriggers[i_c];
      if (trigID >= 44 && trigID <= 100) continue;
      if (trigID >= 111 && trigID <= 113)
	{
	  trignumarg = trigID;
	  continue;
	}
      counterIDs.push_back(trigID);
    }
  if (counterIDs.size() != 2) return false;
  if (trignumarg == 0) return false;

  if (trignumarg == 112 || trignumarg == 113)
    {
      if (fCounterPositionMap[counterIDs[0]].first.X() > fCounterPositionMap[counterIDs[1]].first.X())
	{
	  c1arg = counterIDs[0];
	  c2arg = counterIDs[1];
	}
      else
	{
	  c1arg = counterIDs[1];
	  c2arg = counterIDs[0];
	}
    }
  else if (trignumarg == 111)
    {
      if (fCounterPositionMap[counterIDs[0]].first.Z() > fCounterPositionMap[counterIDs[1]].first.Z())
	{
	  c1arg = counterIDs[0];
	  c2arg = counterIDs[1];
	}
      else
	{
	  c1arg = counterIDs[1];
	  c2arg = counterIDs[0];
	}
    }
  if (c1arg == c2arg) return false;
  if (c1arg == 999 || c2arg == 999) return false;

  return true;
}

float dune::RobustHitFinder::TimeToDriftDist(float thistime, unsigned int thistpc)
{
  float vd = fDetProp->DriftVelocity(fEfield[0]);
  float v1 = fDetProp->DriftVelocity(fEfield[1]);
  float v2 = fDetProp->DriftVelocity(fEfield[2]);
  float v3 = fDetProp->DriftVelocity(fEfield[3]);

  float ld = -1, l1 = -1, l2 = -1, l3 = -1;

  try
    {
      if (thistpc == 1 || thistpc == 3 || thistpc == 5 || thistpc == 7)
	{
	  ld = fGeom->TPC(thistpc).MaxX()-0.511;
	  l1 = 0.511-fGeom->TPC(thistpc).PlaneLocation(0)[0];
	  l2 = fGeom->TPC(thistpc).PlaneLocation(0)[0]-fGeom->TPC(thistpc).PlaneLocation(1)[0];
	  l3 = fGeom->TPC(thistpc).PlaneLocation(1)[0]-fGeom->TPC(thistpc).PlaneLocation(2)[0];
	}
      else if (thistpc == 0 || thistpc == 2 || thistpc == 4 || thistpc == 6)
	{
	  ld = -8.490-fGeom->TPC(thistpc).MinX();
	  l1 = fGeom->TPC(thistpc).PlaneLocation(0)[0]+8.490;
	  l2 = fGeom->TPC(thistpc).PlaneLocation(1)[0]-fGeom->TPC(thistpc).PlaneLocation(0)[0];
	  l3 = fGeom->TPC(thistpc).PlaneLocation(2)[0]-fGeom->TPC(thistpc).PlaneLocation(1)[0];
	}
    }
  catch (cet::exception &e)
    {
      mf::LogError("RobustHitFinder") << e;
      return -99999;
    }

  if (ld < 0 || l1 < 0 || l2 < 0 || l3 < 0) return -99998;

  float t3max = l3/v3;
  float t2max = t3max+(l2/v2);
  float t1max = t2max+(l1/v1);
  float tdmax = t1max+(ld/vd);

  if      (0     <= thistime && thistime < t3max) return v3*thistime;
  else if (t3max <= thistime && thistime < t2max) return l3+v2*(thistime-t3max);
  else if (t2max <= thistime && thistime < t1max) return l3+l2+v1*(thistime-t2max);
  else if (t1max <= thistime && thistime < tdmax) return l3+l2+l1+vd*(thistime-t1max);

  return -99997;
}

float dune::RobustHitFinder::TimeToDisplacement(float thistime)
{
  return thistime*fDetProp->DriftVelocity(fEfield[0]);
}

float dune::RobustHitFinder::TimeToX(float thistime, unsigned int thistpc)
{
  float driftdistance = TimeToDriftDist(thistime,thistpc);
  if (driftdistance < -89999) return driftdistance;
  try
    {
      if (thistpc == 1 || thistpc == 3 || thistpc == 5 || thistpc == 7)
	{
	  return (fGeom->TPC(thistpc).PlaneLocation(2)[0]+driftdistance);
	}
      else if (thistpc == 0 || thistpc == 2 || thistpc == 4 || thistpc == 6)
	{
	  return (fGeom->TPC(thistpc).PlaneLocation(2)[0]-driftdistance);
	}
    }
  catch (cet::exception &e)
    {
      mf::LogError("RobustHitFinder") << e;
    }
  return -99995;
}

float dune::RobustHitFinder::hitGeomDist(TVector3 hitloc, TVector3 trigloc1, TVector3 trigloc2)
{
  return (((hitloc-trigloc1).Cross(hitloc-trigloc2)).Mag()/(trigloc2-trigloc1).Mag());
}

void dune::RobustHitFinder::Reset()
{
  run = -99999;
  event = -99999;
  t0 = -99999;
  c1 = 99999;
  c2 = 99999;
  trignum = 99999;
  c1x = -99999;
  c1y = -99999;
  c1z = -99999;
  c2x = -99999;
  c2y = -99999;
  c2z = -99999;
  channel = -99999;
  tpc = -99999;
  wire = -99999;
  distancecut = -99999;
  signalsize = -99999;
  signal.clear();
  signalFilter.clear();
  baseline = -99999;
  rms = -99999;
  baselineFilter = -99999;
  rmsFilter = -99999;
  pedmean = -99999;
  pedrms = -99999;
  integral = -99999;
  integralFilter = -99999;
  sumADC = -99999;
  sigmaintegral = -99999;
  sigmaintegralFilter = -99999;
  amplitude = -99999;
  amplitudeFilter = -99999;
  peaktick = -99999;
  peaktickFilter = -99999;
  peaktime = -99999;
  peaktimeFilter = -99999;
  begintick = -99999;
  endtick = -99999;
  width = -99999;
  hitx = -99999;
  hity = -99999;
  hitz = -99999;
  hiterrxlo = -99999;
  hiterrxhi = -99999;
  hiterrylo = -99999;
  hiterryhi = -99999;
  hiterrzlo = -99999;
  hiterrzhi = -99999;
  perpdist = -99999;
  hitt = -99999;
  driftdist = -99999;
  countercut = false;
  fitconstant = -99999;
  fitconstanterr = -99999;
  fitlinear = -99999;
  fitlinearerr = -99999;
  fitquadratic = -99999;
  fitquadraticerr = -99999;
  fitchi2 = -99999;
  fitsumsqrresidual = -99999;
  fitndf = -99999;
  fitmle = -99999;
  fitsuccess = false;
  fitrealhit = false;
  assumedhit = false;
  segmentlength = -99999;
  nwiresTPC0 = 0;
  nwiresTPC1 = 0;
  nwiresTPC2 = 0;
  nwiresTPC3 = 0;
  nwiresTPC4 = 0;
  nwiresTPC5 = 0;
  nwiresTPC6 = 0;
  nwiresTPC7 = 0;
  numGoodHitsChan = 0;
}

DEFINE_ART_MODULE(dune::RobustHitFinder)
