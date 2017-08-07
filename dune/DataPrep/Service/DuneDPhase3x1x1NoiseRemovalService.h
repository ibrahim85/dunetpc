// DuneDPhase3x1x1NoiseRemovalService.h
//
// Robert Sulej, Aug 2017
//
// Remove coherent noise from 3x1x1 data.
//

#ifndef DuneDPhase3x1x1NoiseRemovalService_H
#define DuneDPhase3x1x1NoiseRemovalService_H

#include "dune/DuneInterface/AdcNoiseRemovalService.h"
#include "dune/DuneInterface/AdcTypes.h"

namespace geo {
  class Geometry;
}

using GroupChannelMap = std::unordered_map<unsigned int, std::vector<unsigned int> >;

class DuneDPhase3x1x1NoiseRemovalService : public AdcNoiseRemovalService {

public:

  DuneDPhase3x1x1NoiseRemovalService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  int update(AdcChannelDataMap& datamap) const;

  std::ostream& print(std::ostream& out = std::cout, std::string prefix = "") const;

private:

  void removeCoherent(const GroupChannelMap & ch_groups, AdcChannelDataMap& datamap) const;
  void removeLowFreq(AdcChannelDataMap& datamap) const;

  std::vector<bool> roiMask(const AdcChannelData & adc) const;

  /// Make groups of channels using 3x1x1 DAQ numbering. Channels tagged as noisy are excluded at this stage.
  GroupChannelMap makeDaqGroups(size_t gsize) const;
  /// Make groups of channels using LArSoft numbering. Channels tagged as noisy are excluded at this stage.
  GroupChannelMap makeGroups(size_t gsize) const;
  
  /// Get 3x1x1 DAQ channel number from the LArSoft's channel index.
  static size_t get311Chan(size_t LAr_chan);

  // Configuration parameters.
  bool fCoherent, fLowFreq;
  float fRoiStartThreshold;
  float fRoiEndThreshold;
  int fRoiPadLow;
  int fRoiPadHigh;

  // Services.
  const geo::Geometry* fGeometry;
};

DECLARE_ART_SERVICE_INTERFACE_IMPL(DuneDPhase3x1x1NoiseRemovalService, AdcNoiseRemovalService, LEGACY)

#endif
