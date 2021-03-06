////////////////////////////////////////////////////////////////////////
// Class:       FilterAnalyzerRunList
// Module Type: analyser
// File:        FilterAnalyzerRunList_module.cc
// Author:      Karl Warburton (k.warburton@sheffield.ac.uk), April 2016
// 
// A quick analysis module for looking at the filter.
//
// Runs over an artdaq-formatted file and produces a tree or histogram,
// filled for each channel combination and the respective correlation.
////////////////////////////////////////////////////////////////////////

// framework
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// lbne-artdaq
#include "lbne-raw-data/Overlays/TpcMilliSliceFragment.hh"
#include "artdaq-core/Data/Fragment.hh"

// larsoft
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "larcore/Geometry/Geometry.h"
#include "tpcFragmentToRawDigits.h"
#include "utilities/UnpackFragment.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "dune/RunHistory/DetPedestalDUNE.h"
#include "cetlib/getenv.h"

// c++
#include <memory>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>

// ROOT
#include "TMath.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TVirtualFFT.h"
#include "TStyle.h"

namespace DAQToOffline {
  class FilterAnalyzerRunList;
}

class DAQToOffline::FilterAnalyzerRunList : public art::EDAnalyzer {
public:

  explicit FilterAnalyzerRunList(fhicl::ParameterSet const & pset);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  FilterAnalyzerRunList(FilterAnalyzerRunList const &) = delete;
  FilterAnalyzerRunList(FilterAnalyzerRunList &&) = delete;
  FilterAnalyzerRunList & operator = (FilterAnalyzerRunList const &) = delete;
  FilterAnalyzerRunList & operator = (FilterAnalyzerRunList &&) = delete;

  void analyze(art::Event const& evt) override;
  void reconfigure(const fhicl::ParameterSet &pset);

private:

  std::string fDigitModuleLabel;
  std::string fDigitModuleInstance;

  TF1* fColFilterFunc;  ///< Parameterized collection filter function.
  TF1* fIndUFilterFunc; ///< Parameterized induction filter function.
  TF1* fIndVFilterFunc; ///< Parameterized induction filter function.

  const lariov::DetPedestalProvider& fPedestalRetrievalAlg = *(lar::providerFrom<lariov::DetPedestalService>());

  int ThisRunSeq = 0;
  std::vector<int> MyUsefulChans;

  TH2F* RawFFT_Planes;
  TH2F* FixFFT_Planes;
  TH2F* RawFFT_APAs;
  TH2F* FixFFT_APAs;

  TH2F* FFTRaw[36];
  TH2F* FFTFix[36];

  int NumRuns = 119;
};

DAQToOffline::FilterAnalyzerRunList::FilterAnalyzerRunList(fhicl::ParameterSet const & pset) : art::EDAnalyzer(pset) {
  
  this->reconfigure(pset);
  gStyle->SetOptStat(0);

  MyUsefulChans.push_back(10);  MyUsefulChans.push_back(70); MyUsefulChans.push_back(110);
  MyUsefulChans.push_back(160); MyUsefulChans.push_back(205); MyUsefulChans.push_back(260);
  MyUsefulChans.push_back(300); MyUsefulChans.push_back(410); MyUsefulChans.push_back(500);

  MyUsefulChans.push_back(525); MyUsefulChans.push_back(585); MyUsefulChans.push_back(625);
  MyUsefulChans.push_back(675); MyUsefulChans.push_back(720); MyUsefulChans.push_back(775);
  MyUsefulChans.push_back(815); MyUsefulChans.push_back(915); MyUsefulChans.push_back(1015);

  MyUsefulChans.push_back(1040); MyUsefulChans.push_back(1095); MyUsefulChans.push_back(1135);
  MyUsefulChans.push_back(1185); MyUsefulChans.push_back(1230); MyUsefulChans.push_back(1285);
  MyUsefulChans.push_back(1325); MyUsefulChans.push_back(1435); MyUsefulChans.push_back(1525);

  MyUsefulChans.push_back(1545); MyUsefulChans.push_back(1605); MyUsefulChans.push_back(1645);
  MyUsefulChans.push_back(1695); MyUsefulChans.push_back(1740); MyUsefulChans.push_back(1795);
  MyUsefulChans.push_back(1835); MyUsefulChans.push_back(1945); MyUsefulChans.push_back(2035);
  
  std::cout << "I have made my useful channel vector. It has size " << MyUsefulChans.size() << ". It's elements are as follows." << std::endl;
  for (size_t xx=0; xx<MyUsefulChans.size(); ++xx) {
    std::cout << "MyUsefulChans["<<xx<<"] " << MyUsefulChans[xx] << std::endl;
  }
  
  art::ServiceHandle<art::TFileService> tfs;
  
  RawFFT_Planes = tfs->make<TH2F>("RawFFT_Planes", "Raw FFT for all channels grouped by plane; Channel Number; Frequency (KHz)", 36, 0, 36, 1000, 0, 1000 );
  FixFFT_Planes = tfs->make<TH2F>("FixFFT_Planes", "Raw FFT for all channels grouped by plane; Channel Number; Frequency (KHz)", 36, 0, 36, 1000, 0, 1000 );

  RawFFT_APAs = tfs->make<TH2F>("RawFFT_APAs", "Raw FFT for all channels grouped by plane; Channel Number; Frequency (KHz)", 36, 0, 36, 1000, 0, 1000 );
  FixFFT_APAs = tfs->make<TH2F>("FixFFT_APAs", "Raw FFT for all channels grouped by plane; Channel Number; Frequency (KHz)", 36, 0, 36, 1000, 0, 1000 );
  
  for (int XBin=0; XBin<36; ++XBin) {
    std::stringstream oss;
    oss<<"Chan_"<<MyUsefulChans[XBin];
    int WhAPA = XBin/9;
    int WhPla = (XBin/3)%3;
    int PlaIn = XBin%3;
    RawFFT_Planes->GetXaxis()->SetBinLabel((WhAPA*3)+(WhPla*12)+PlaIn+1, oss.str().c_str());
    FixFFT_Planes->GetXaxis()->SetBinLabel((WhAPA*3)+(WhPla*12)+PlaIn+1, oss.str().c_str());

    RawFFT_APAs->GetXaxis()->SetBinLabel(XBin+1, oss.str().c_str());
    FixFFT_APAs->GetXaxis()->SetBinLabel(XBin+1, oss.str().c_str());
  }

  for (size_t HistoChan=0; HistoChan<MyUsefulChans.size(); HistoChan++) {
    std::stringstream oss1a, oss1b;
    oss1a << "RawFFT_"<<HistoChan;
    oss1b << "Raw FFT for Channel "<<MyUsefulChans[HistoChan]<<"; Run Number; Frequency (KHz)";
    FFTRaw[HistoChan] = tfs->make<TH2F>(oss1a.str().c_str(), oss1b.str().c_str(), NumRuns, 0, NumRuns, 1000, 0, 1000);

    std::stringstream oss2a, oss2b;
    oss2a << "FixFFT_"<<HistoChan;
    oss2b << "Fix FFT for Channel "<<MyUsefulChans[HistoChan]<<"; Run Number; Frequency (KHz)";
    FFTFix[HistoChan] = tfs->make<TH2F>(oss2a.str().c_str(), oss2b.str().c_str(), NumRuns, 0, NumRuns, 1000, 0, 1000);
  }
  
  
}

void DAQToOffline::FilterAnalyzerRunList::reconfigure(fhicl::ParameterSet const& pset) {
  fDigitModuleLabel    = pset.get<std::string>("DigitModuleLabel");
  fDigitModuleInstance = pset.get<std::string>("DigitModuleInstance");

  // Make the filter functions.
  std::string colFilt               = pset.get<std::string>("ColFilter");
  std::vector<double> colFiltParams = pset.get<std::vector<double> >("ColFilterParams");
  fColFilterFunc = new TF1("colFilter", colFilt.c_str());
  for(unsigned int i=0; i<colFiltParams.size(); ++i)
    fColFilterFunc->SetParameter(i, colFiltParams[i]);
    
  std::string indUFilt               = pset.get<std::string>("IndUFilter");
  std::vector<double> indUFiltParams = pset.get<std::vector<double> >("IndUFilterParams");
  fIndUFilterFunc = new TF1("indUFilter", indUFilt.c_str());
  for(unsigned int i=0; i<indUFiltParams.size(); ++i)
    fIndUFilterFunc->SetParameter(i, indUFiltParams[i]);

  std::string indVFilt               = pset.get<std::string>("IndVFilter");
  std::vector<double> indVFiltParams = pset.get<std::vector<double> >("IndVFilterParams");
  fIndVFilterFunc = new TF1("indVFilter", indVFilt.c_str());
  for(unsigned int i=0; i<indVFiltParams.size(); ++i)
    fIndVFilterFunc->SetParameter(i, indVFiltParams[i]);
}

void DAQToOffline::FilterAnalyzerRunList::analyze(art::Event const& evt) {
  if (evt.event() != 1) return;

  ++ThisRunSeq;
  std::cout << "\n\n\n\nThis Run sequence is " << ThisRunSeq << "\n\n\n" << std::endl;
  for (size_t HistoChan=0; HistoChan<MyUsefulChans.size(); HistoChan++) {
    std::stringstream oss;
    oss<<"Run "<<(int)evt.run();    
    FFTRaw[HistoChan]->GetXaxis()->SetBinLabel(ThisRunSeq, oss.str().c_str());
    FFTFix[HistoChan]->GetXaxis()->SetBinLabel(ThisRunSeq, oss.str().c_str());
  }

  art::ServiceHandle<geo::Geometry> geo;

  art::Handle<std::vector<raw::RawDigit> > rawDigitHandle;
  evt.getByLabel(fDigitModuleLabel, fDigitModuleInstance, rawDigitHandle);
  std::vector<raw::RawDigit> const& rawDigitVector(*rawDigitHandle);
 
  
  std::vector< std::pair<int,int> > ZeroFreq;
  //ZeroFreq.push_back( std::make_pair(276 , 285 ) );
  //ZeroFreq.push_back( std::make_pair(558 , 568 ) );
  //ZeroFreq.push_back( std::make_pair(837 , 849 ) );
  //ZeroFreq.push_back( std::make_pair(1116, 1127) );
  //ZeroFreq.push_back( std::make_pair(4340, 5205) );

  for (size_t DigLoop=0; DigLoop < rawDigitVector.size(); ++DigLoop) {
    int Channel     = rawDigitVector[DigLoop].Channel();
    size_t NADC     = rawDigitVector[DigLoop].NADC();
    double Pedestal = rawDigitVector[DigLoop].GetPedestal();
    const geo::View_t view = geo->View(Channel);
    // Check if this channel is one of the ones I want.
    for (size_t GotChan=0; GotChan<MyUsefulChans.size(); ++GotChan) {
      if (MyUsefulChans[GotChan] != Channel) continue;

      int WhAPA = GotChan/9;
      int WhPla = (GotChan/3)%3;
      int PlaIn = GotChan%3;
      //std::cout << "Looking at Chan " << Channel << ", GotChan " << GotChan << ". I am going to fill bin " << (WhAPA*3)+(WhPla*12)+PlaIn+0.5 << std::endl;
      
      //std::cout << "Looking at rawDigitVector["<<DigLoop<<"] it was on channel " << rawDigitVector[DigLoop].Channel() << "("<<Channel<<") it is in View " << view
      //	<< ", NADC is " << rawDigitVector[DigLoop].NADC() << " ("<<NADC<<")"
      //	<< ", pedestal is " << rawDigitVector[DigLoop].GetPedestal() << " ("<<Pedestal<<")"
      //	<< std::endl;
      
      // Fill the RawDigit histogram for this histogram.
      TH1F* hRawDigit = new TH1F("hRawDigit","",NADC,0,NADC/2);
      TH1F* hRawFFT   = new TH1F("hRawFFT"  ,"",NADC,0,NADC);
      for (size_t ADCs=0; ADCs < NADC; ++ADCs) {
	hRawDigit -> SetBinContent( ADCs+1, rawDigitVector[DigLoop].ADC(ADCs)-Pedestal );
      }
      for (size_t ww=NADC; ww<NADC; ++ww)
	hRawFFT -> SetBinContent( ww, 0 );
      // Make the FFT for this channel.
      hRawDigit -> FFT( hRawFFT ,"MAG");
      for (size_t bin = 0; bin < NADC; ++bin) {
	double BinVal = hRawFFT->GetBinContent(bin+1);
	double freq = 2000. * bin / (double)NADC;
	if (freq < 1000 && BinVal < 1e5) {
	  FFTRaw[GotChan] -> Fill( (ThisRunSeq-0.5) , freq, BinVal );
	  RawFFT_Planes -> Fill( (WhAPA*3)+(WhPla*12)+PlaIn+0.5, freq, BinVal );
	  RawFFT_APAs   -> Fill( GotChan+0.5, freq, BinVal );
	}
      }
      
      // I want to do an inverse FFT, so need to convert the tranformed FFT into an array....
      //double Re[NADC], Im[NADC];
      std::unique_ptr<double[]> Re( new double[NADC]);
      std::unique_ptr<double[]> Im( new double[NADC]);
      TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
      fft->GetPointsComplex(Re.get(),Im.get());
      
      // Set the noisy frequency range bins to an average value.
      for (size_t aa=0; aa<ZeroFreq.size(); ++aa) {
	for (int bb=ZeroFreq[aa].first; bb<ZeroFreq[aa].second; ++bb) {
	  double ReMeanVal=0;
	  double ImMeanVal=0;
	  int Range = 50;
	  for (int cc=0; cc<Range; ++cc) {
	    ReMeanVal += Re[ZeroFreq[aa].first-cc] + Re[ZeroFreq[aa].second+cc];
	    ImMeanVal += Im[ZeroFreq[aa].first-cc] + Im[ZeroFreq[aa].second+cc];
	  }
	  ReMeanVal = ReMeanVal / Range;
	  Re[bb]    = Re[1500-bb] = ReMeanVal;
	  ImMeanVal = ImMeanVal / Range;
	  Im[bb]    = Im[1500-bb] = ImMeanVal;
	}
      }
      Re[0] = Re[1];
      Im[0] = Im[1];
          
      // Apply the filter...    
      for (size_t bin = 0; bin < NADC; ++bin) {
	double freq = 2000. * bin / NADC;
	if (view == geo::kU) { // U plane 
	  Re[bin] = Re[bin]*fIndUFilterFunc->Eval(freq);
	  Im[bin] = Im[bin]*fIndUFilterFunc->Eval(freq);
	} else if ( view == geo::kV) { // V plane
	  Re[bin] = Re[bin]*fIndVFilterFunc->Eval(freq);
	  Im[bin] = Im[bin]*fIndVFilterFunc->Eval(freq);
	} else if ( view == geo::kZ) { // Collection plane
	  Re[bin] = Re[bin]*fColFilterFunc->Eval(freq);
	  Im[bin] = Im[bin]*fColFilterFunc->Eval(freq);
	}
	double MagVal =  pow ( Re[bin]*Re[bin] + Im[bin]*Im[bin], 0.5);
	if (TMath::IsNaN(MagVal)) MagVal = 0;
	            
	// Now do the big histograms...
	if (freq < 1000 && MagVal < 1e5) {
	  FFTFix[GotChan] -> Fill( (ThisRunSeq-0.5) , freq, MagVal );
	  FixFFT_Planes -> Fill( (WhAPA*3)+(WhPla*12)+PlaIn+0.5, freq, MagVal );
	  FixFFT_APAs   -> Fill( GotChan+0.5, freq, MagVal );
	}
      }
    } // Checking if one of my chosen useful channels
  } // RawDigit Loop
  return;
}

DEFINE_ART_MODULE(DAQToOffline::FilterAnalyzerRunList)
