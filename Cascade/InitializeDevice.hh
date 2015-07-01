#ifndef _InitializeDevice_hh_
#define _InitializeDevice_hh_
#include "MeasurementStruct.hh"
#include "TUniverseCDAQBox.hh"

namespace cascade {


boost::any StartMeasurement();
boost::any GetCurrentStatus();
boost::any StopMeasurement();

class InitializeDevice {
  private:
  InitializeDevice();
  InitializeDevice(const InitializeDevice&);
  InitializeDevice& operator=(const InitializeDevice&);

  public:

  static InitializeDevice& Device();
  TUniverseCDAQBox _DAQBox;
  CurrentStatus _Status;
  MeasurementData _MeasurementData;
};

}

#endif /*_InitializeDevice_hh_ */
