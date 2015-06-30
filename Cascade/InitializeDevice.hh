#ifndef _InitializeDevice_hh_
#define _InitializeDevice_hh_
#include "MeasurementStruct.hh"
#include "TUniverseCDAQBox.hh"

namespace cascade {

class InitializeDevice {
  public:
  InitializeDevice();

  public:
  TUniverseCDAQBox _DAQBox;
  CurrentStatus _Status;
  MeasurementData _MeasurementData;
};

}

#endif /*_InitializeDevice_hh_ */
