////////////////////////////////////////////////////////////////////////
// Class:       SSPToOffline
// Module Type: producer
// File:        SSPToOffline_module.cc
//
// Repackage raw SSP data into OpDetPulse data products
//
// Alex Himmel ahimme@phy.duke.edu
//
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <iostream>

//lbne-artdaq includes
#include "lbne-raw-data/Overlays/SSPFragment.hh"
#include "artdaq-core/Data/Fragments.hh"

//larsoft includes
#include "RawData/raw.h"
#include "RawData/OpDetPulse.h"
#include "Geometry/Geometry.h"

#include "utilities/UnpackFragment.h"

namespace DAQToOffline {
    class SSPToOffline;
}

class DAQToOffline::SSPToOffline : public art::EDProducer {
public:
    explicit SSPToOffline(fhicl::ParameterSet const & pset);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    SSPToOffline(SSPToOffline const &) = delete;
    SSPToOffline(SSPToOffline &&) = delete;
    SSPToOffline & operator = (SSPToOffline const &) = delete;
    SSPToOffline & operator = (SSPToOffline &&) = delete;
    void produce(art::Event & evt) override;
    void reconfigure(fhicl::ParameterSet const& pset);
    void printParameterSet();

private:

    std::string fFragType;
    std::string fRawDataLabel;
    std::string fOutputDataLabel;
    //bool fDebug;
    //raw::Compress_t        fCompression;      ///< compression type to use
    //unsigned int           fZeroThreshold;    ///< Zero suppression threshold


};


DAQToOffline::SSPToOffline::SSPToOffline(fhicl::ParameterSet const & pset)
{

    this->reconfigure(pset);

    produces< std::vector<raw::OpDetPulse> > (fOutputDataLabel);  

}

void DAQToOffline::SSPToOffline::reconfigure(fhicl::ParameterSet const& pset){

    fFragType = pset.get<std::string>("FragType");
    fRawDataLabel = pset.get<std::string>("RawDataLabel");
    fOutputDataLabel = pset.get<std::string>("OutputDataLabel");
    //fDebug = pset.get<bool>("Debug");

    //fZeroThreshold=0;
    //fCompression=raw::kNone;
    printParameterSet();

}

void DAQToOffline::SSPToOffline::printParameterSet(){

    mf::LogDebug("SSPToOffline") << "====================================" << "\n"
                                 << "Parameter Set" << "\n"
                                 << "====================================" << "\n"
                                 << "fFragType:        " << fFragType << "\n"
                                 << "fRawDataLabel:    " << fRawDataLabel << "\n"
                                 << "fOutputDataLabel: " << fOutputDataLabel << "\n"
                                 << "====================================" << "\n";
}

void DAQToOffline::SSPToOffline::produce(art::Event & evt)
{


    art::Handle<artdaq::Fragments> raw;
    evt.getByLabel(fRawDataLabel, fFragType, raw);

    std::unique_ptr< std::vector<raw::OpDetPulse> > opDetPulseVector(new std::vector <raw::OpDetPulse > );  
  
    art::EventNumber_t eventNumber = evt.event();

    //Check that the data is valid
    if(!raw.isValid()){
        mf::LogError("SSPToOffline") << "Run: " << evt.run()
                  << ", SubRun: " << evt.subRun()
                  << ", Event: " << eventNumber
                  << " is NOT VALID";
        throw cet::exception("raw NOT VALID");
        return;
    }
  


    mf::LogDebug("SSPToOffline") << "Run: " << evt.run()
                                 << ", SubRun: " << evt.subRun()
                                 << ", Event: " << eventNumber
                                 << " has " << raw->size()
                                 << " rawFragments";



    for (size_t idx = 0; idx < raw->size(); ++idx) {
        const auto& frag((*raw)[idx]);

        lbne::SSPFragment sspf(frag);

        mf::LogDebug("SSPToOffline") << "\n"
                                     << "SSP fragment "     << frag.fragmentID() 
                                     << " has total size: " << sspf.hdr_event_size()
                                     << " and run number: " << sspf.hdr_run_number()
                                     << " with " << sspf.total_adc_values() << " total ADC values"
                                     << "\n"
                                     << "\n";

        //
        // The elements of the OpDet Pulse
        //
        unsigned short     OpChannel = -1;     ///< channel in the readout
        std::vector<short> Waveform;           ///< vector of ADC counts
        unsigned int       PMTFrame = 0;       ///< frame number where pulse begins
        unsigned int       FirstSample = 0;    ///< first sample number within pmt frame (time in us?)
        

        
        //get the information from the header
        if(frag.hasMetadata())
        {
            const lbne::SSPFragment::Metadata * meta = frag.metadata<lbne::SSPFragment::Metadata>();
            // peaksum arrives as a signed 24 bit number.  It must be sign extended when stored as a 32 bit int 
            uint32_t peaksum = ((meta->daqHeader.group3 & 0x00FF) >> 16) + meta->daqHeader.peakSumLow;
            if(peaksum & 0x00800000) {
                peaksum |= 0xFF000000;
            }
            mf::LogDebug("SSPToOffline")
                << "Header:                             " << meta->daqHeader.header   << "\n"
                << "Length:                             " << meta->daqHeader.length   << "\n"
                << "Trigger type:                       " << ((meta->daqHeader.group1 & 0xFF00) >> 8) << "\n"
                << "Status flags:                       " << ((meta->daqHeader.group1 & 0x00F0) >> 4) << "\n"
                << "Header type:                        " << ((meta->daqHeader.group1 & 0x000F) >> 0) << "\n"
                << "Trigger ID:                         " << meta->daqHeader.triggerID << "\n"
                << "Module ID:                          " << ((meta->daqHeader.group2 & 0xFFF0) >> 4) << "\n"
                << "Channel ID:                         " << ((meta->daqHeader.group2 & 0x000F) >> 0) << "\n"
                << "Sync delay:                         " << ((unsigned int)(meta->daqHeader.timestamp[1]) << 16) + (unsigned int)(meta->daqHeader.timestamp[0]) << "\n"
                << "Sync count:                         " << ((unsigned int)(meta->daqHeader.timestamp[3]) << 16) + (unsigned int)(meta->daqHeader.timestamp[2]) << "\n"
                << "Peak sum:                           " << peaksum << "\n"
                << "Peak time:                          " << ((meta->daqHeader.group3 & 0xFF00) >> 8) << "\n"
                << "Prerise:                            " << ((meta->daqHeader.group4 & 0x00FF) << 16) + meta->daqHeader.preriseLow << "\n"
                << "Integrated sum:                     " << ((unsigned int)(meta->daqHeader.intSumHigh) << 8) + (((unsigned int)(meta->daqHeader.group4) & 0xFF00) >> 8) << "\n"
                << "Baseline:                           " << meta->daqHeader.baseline << "\n"
                << "CFD Timestamp interpolation points: " << meta->daqHeader.cfdPoint[0] << " " << meta->daqHeader.cfdPoint[1] << " " << meta->daqHeader.cfdPoint[2] << " " << meta->daqHeader.cfdPoint[3] << "\n"
                << "Internal interpolation point:       " << meta->daqHeader.intTimestamp[0] << "\n"
                << "Internal timestamp:                 " << ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[3] << 32)) + ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[2] << 16)) + ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[1] << 0)) << "\n"
                << "\n";

            // Store the channel number
            OpChannel = ((meta->daqHeader.group2 & 0x000F) >> 0);

            // Store the time of the first sample ? TK: GUESS
            FirstSample = ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[3] << 32)) + ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[2] << 16)) + ((uint64_t)((uint64_t)meta->daqHeader.intTimestamp[1] << 0)) ;
        }
        else
        {
            mf::LogWarning("SSPToOffline") << "SSP fragment has no metadata associated with it." << "\n";
        }


        for(size_t idata = 0; idata < sspf.total_adc_values(); idata++) {
            //const lbne::SSPFragment::adc_t * adc = sspf.dataBegin() + idata;
            const uint16_t * adc = (const uint16_t*)sspf.dataBegin() + idata;
            Waveform.push_back(*adc);

        }

	opDetPulseVector->push_back( raw::OpDetPulse( OpChannel, Waveform, PMTFrame, FirstSample ) );
    }
    evt.put(std::move(opDetPulseVector), fOutputDataLabel);


}

DEFINE_ART_MODULE(DAQToOffline::SSPToOffline)

