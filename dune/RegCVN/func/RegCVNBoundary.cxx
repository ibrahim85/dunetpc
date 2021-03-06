////////////////////////////////////////////////////////////////////////
/// \file    RegCVNBoundary.cxx
/// \brief   RegCVNBoundary for RegCVN PixelMap modified from CVNBoundary.cxx
/// \author  Ilsoo Seong - iseong@uci.edu
////////////////////////////////////////////////////////////////////////

#include  <iostream>
#include  <ostream>
#include  <utility>
#include  <cassert>


#include "dune/RegCVN/func/RegCVNBoundary.h"

namespace cvn
{

  RegCVNBoundary::RegCVNBoundary(const int& nWire, // # of wires of pixel map
                     const int& nTDC, // # of TDC of pixel map
		     const int& tRes, // # of TDC merging
                     const int& WireMeanX,
                     const int& WireMeanY,
                     const int& WireMeanZ,
                     const int& TDCMeanX,
                     const int& TDCMeanY,
                     const int& TDCMeanZ):
    fFirstWire{WireMeanX-nWire/2,
      WireMeanY-nWire/2,
      WireMeanZ-nWire/2},
    fLastWire{WireMeanX+nWire/2,
      WireMeanY+nWire/2,
      WireMeanZ+nWire/2},
    fFirstTDC{TDCMeanX - (nTDC*tRes)/2-tRes,
        TDCMeanY  - (nTDC*tRes)/2-tRes,
        TDCMeanZ  - (nTDC*tRes)/2-tRes},  
    fLastTDC{TDCMeanX + (nTDC*tRes)/2,
        TDCMeanY + (nTDC*tRes)/2,
        TDCMeanZ + (nTDC*tRes)/2}
  
  {
    assert(fLastWire[0] - fFirstWire[0] == nWire - 1);
    assert(fLastWire[1] - fFirstWire[1] == nWire - 1);
    assert(fLastWire[2] - fFirstWire[2] == nWire - 1);
  }

  bool RegCVNBoundary::IsWithin(const int& wire, const int& tdc, const unsigned int& view)
  {
    bool inWireRcvne = (int) wire >= fFirstWire[view] && (int) wire < fLastWire[view];
    bool inTDCRcvne = (int) tdc > fFirstTDC[view] &&
                      (int) tdc < fLastTDC[view];
    return inWireRcvne && inTDCRcvne;
  }


  std::ostream& operator<<(std::ostream& os, const RegCVNBoundary& b)
  {
    os<<"RegCVNBoundary with "
      <<"(first,last) wire X: (" << b.FirstWire(0)<<", "<< b.LastWire(0)
      <<"(first,last) wire Y: (" << b.FirstWire(1)<<", "<< b.LastWire(1)
      <<"(first,last) wire Z: (" << b.FirstWire(2)<<", "<< b.LastWire(2)
      <<"), (first,last) tdc X: ("<<b.FirstTDC(0)<<", "<<b.LastTDC(0)<<")"
      <<"), (first,last) tdc Y: ("<<b.FirstTDC(1)<<", "<<b.LastTDC(1)<<")"
      <<"), (first,last) tdc Z: ("<<b.FirstTDC(2)<<", "<<b.LastTDC(2)<<")";

    return os;
  }
}
