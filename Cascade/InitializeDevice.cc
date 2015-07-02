#include "InitializeDevice.hh"
//#include "PrintStruct.hh"
#include "ReturnCheck.hh"
#include "Error.hh"
#include <iostream>
#include "universe_api.h"
#include "StructConversion.hh"
#include "BuildLambdaFunction.hh"


namespace cascade {

InitializeDevice::InitializeDevice()
{
  RegisterTypeCheck<DWORD>([this] (const DWORD& at) {
    if (at != EC_OK) {
      throw cascade::daqbox_error(_DAQBox.GetErrorText(at));
    }
    return at;
  });

  set_hw_byte_swap(true);
  // get version of the used HardwareLib and for wich OS it is compiled for
  std::cout << _DAQBox.GetVersionHardwareLib() << std::endl;
  std::cout << _DAQBox.GetVersionOS() << std::endl;

  // init the USB interface of the DAQBox
  std::cout << "0x" << std::hex << PerformCheck(_DAQBox.Init( 0x440000 ))
            << " : DAQBox Init" << std::endl;
  _MeasurementData.firmwareVersion = _DAQBox.GetFirmwareVersion();
  // reset DAQBox, if wanted or needed
  _DAQBox.Reset();

  // get firmware version of DAQBox
  std::cout << _DAQBox.GetFirmwareVersion() << std::endl;

  // get status register
  DWORD status;
  std::cout << "0x" << std::hex << _DAQBox.GetStatusRegister(&status) << " : DAQBox get status" << std::endl;
  std::cout << "0x" << std::hex << status << std::endl << std::endl;

  TUniverseCDAQBox* p_cf = &_DAQBox;
  TUniverseCDAQBox* p_m = &_DAQBox;

  HSetup setup = _DAQBox.GetActualHardwareSetup();
  PerformCheck(_DAQBox.SetHardwareSetup(setup));


  CFConfig cconfig = p_cf->GetActualConfigurationOfFirmware();

  // Configuration register of the filter algorithm for event reconstruction in time and space.
  // ConfigReg = AntiAll(5+1+(NO_OF_CIPIX/2) - 0) |
  //              AntiCorner(2 - 0) | TakeNoGainBit |
  //              TakeBrokenBit | GEMTimeBit | NoFilterBit | ResetBit
  // Further details under section "Filter Entity Reference" of CASCADE firmware manuals.
  cconfig.dwEventFilterConfig = 0x1040;
  // type of list mode data: raw data (RAW) or filtered (FILTERED) data
  cconfig.dwTypeOfListModeData = RAW;
  // dis/enable Slave mode
  cconfig.bSlaveMode = false;

  /// Which INPUT( No = 0, 1, 2, ... ) will be connected to which internal signal 'nSignalIn'.
  /// Further details under section "DigitalIO Entity Reference" of CASCADE firmware manuals.
  for (int i = 0; i < 4; i++ )
      cconfig.dwInputMultiplexer[i] = i;
  /// Which OUTPUT( No = 0, 1, 2, ... ) will be connected to which internal Signal 'nSignalOut'.
  /// Further details under section "DigitalIO Entity Reference" of CASCADE firmware manuals.
  for (int i = 0; i < 4; i++ )
      cconfig.dwOutputMultiplexer[i] = i;
  /// Length of TOF output signals (Chopper, NewTimeBin) in terms of 25ns (1 - 2^24-1) generated
  /// from firmware.
  cconfig.dwLengthTOFOut = 1000;

  PerformCheck(p_cf->ConfigureFirmware( cconfig ));



  //--------------------- Configuration of a Simulation -----------------------------------------

  CFSimulation simulation;
  simulation.bSimulationCIPix = true;
  simulation.dwNoOfPauseCycles = 2;

  // j = 0 (low-word CIPix 0), 1 (high-word CIPix 0), 2 (low-word CIPix 1), etc.
  DWORD j = 0;
  simulation.dwXSimulationPattern[0][j] = 0x00000001;
  simulation.dwXSimulationPattern[1][j] = 0x00000000;
  simulation.dwXSimulationPattern[2][j] = 0x80000000;
  simulation.dwXSimulationPattern[3][j] = 0x00000008;
  simulation.dwXSimulationPattern[4][j] = 0x00000000;
  simulation.dwXSimulationPattern[5][j] = 0x00000000;
  simulation.dwXSimulationPattern[6][j] = 0x00000008;
  simulation.dwXSimulationPattern[7][j] = 0x00000008;

  simulation.dwYSimulationPattern[0][j] = 0x00000001;
  simulation.dwYSimulationPattern[1][j] = 0x00000000;
  simulation.dwYSimulationPattern[2][j] = 0x80000000;
  //...

  simulation.bSimulationChopper = false;
  simulation.dwChopperPeriod = 1000000; // 100ms

  PerformCheck(p_cf->ConfigureFirmwareSimulation( simulation ));

  MConfig config = p_m->GetActualConfigurationOfMeasurement();

  // config of the measurement type
  config.dwTypeOfReadout = ONE_D_STRIPES;
  config.dwTypeOfMeasurement = IMAGE;
  config.dwTypeOfMeasurement = TOF;
  config.dwXResolution = 1;
  config.dwYResolution = 1;

  // measurement time in terms of 100ns
  config.ullTimeToCount = 1*10000000; // 1 sec

  // prameters for TOF
  // dwelltime in terms of 100ns
  config.dwDwellTime = 10000; // 1 msec
  config.dwNoOfTimeBins = 1;
  config.bBreakMode = true;

  PerformCheck(p_m->ConfigureMeasurement( config ));

  _Status.bins.time = _MeasurementData.measurementConfig.dwNoOfTimeBins;
  _Status.bins.x = _MeasurementData.measurementConfig.dwXResolution;
  _Status.bins.y = _MeasurementData.measurementConfig.dwYResolution;
  _Status.Reset();

  PerformCheck(_DAQBox.InitDataBuffer(&_Status.data[0]));
  PerformCheck(_DAQBox.ClearSRAM());
  PerformCheck(_DAQBox.StartMeasurement());


}

//_____________________________________________________________________________
InitializeDevice& InitializeDevice::Device()
{
  static InitializeDevice fgDev;
  return fgDev;
}

//_____________________________________________________________________________
// Structs to hold status and measurement
InitializeDevice& gDev = InitializeDevice::Device();
TUniverseCDAQBox& gDAQBox = gDev._DAQBox;
CurrentStatus& gStatus = gDev._Status;
MeasurementData& gMeasurementData = gDev._MeasurementData;

//_____________________________________________________________________________
using cascade::detail::anyvec;
using cascade::detail::anymap;

//_____________________________________________________________________________
boost::any StartMeasurement()
{
  // Check if we're running
  if (gStatus.isMeasuring) {
    throw cascade::daqbox_error("Card running");
  }
  gStatus.isMeasuring = true;

  // Grab the current setup
  gMeasurementData.firmwareConfig = gDAQBox.GetActualConfigurationOfFirmware();
  gMeasurementData.hardwareConfig = gDAQBox.GetActualHardwareSetup();
  gMeasurementData.measurementConfig = gDAQBox.GetActualConfigurationOfMeasurement();
  gMeasurementData.simulationConfig = gDAQBox.GetActualConfigurationOfFirmwareSimulation();

  // Allocate the correct amount of data
  gStatus.bins.time = gMeasurementData.measurementConfig.dwNoOfTimeBins;
  gStatus.bins.x = gMeasurementData.measurementConfig.dwXResolution;
  gStatus.bins.y = gMeasurementData.measurementConfig.dwYResolution;
  gStatus.Reset();

  PerformCheck(gDAQBox.InitDataBuffer(&gStatus.data[0]));
  PerformCheck(gDAQBox.StartMeasurement());
  return true;
}

//_____________________________________________________________________________
boost::any GetCurrentStatus()
{
  // Get Current Status of measurement (or card)
  gDAQBox.Measurement();
  gStatus.absTimeElapsed = gDAQBox.GetAbsTimeElapsed();
  DWORD status;
  PerformCheck(gDAQBox.GetStatusRegister(&status));
  gStatus.isMeasuring = ((status & 0x8) == 0);
  return gStatus.encode();
}

//_____________________________________________________________________________
boost::any StopMeasurement()
{
  // Check if we're running
  PerformCheck(gDAQBox.StopMeasurement());
  // Grab the current setup

  bool stopped = false;
  while(!stopped) {
    PerformCheck(gDAQBox.HasMeasurementStopped(&stopped));
  }

  PerformCheck(gDAQBox.ReadStatistics(&gMeasurementData.statistics));
  PerformCheck(gDAQBox.ReadDiagnostics(&gMeasurementData.diagnostics));

  gMeasurementData.lifeTimeElapsed = gDAQBox.GetLifeTimeElapsed();
  gMeasurementData.eventsCounted = gDAQBox.GetEventsCounted();
  gMeasurementData.sweepsElapsed = gDAQBox.GetSweepsElapsed();
  gMeasurementData.chopperFrequency = gDAQBox.GetChopperFrequency();

  anymap config = boost::any_cast<anymap>(gMeasurementData.encode());
  if (!gMeasurementData.simulationConfig.bSimulationCIPix) {
    // Remove from the config
    config.erase("simulationConfig");
  }
  config["status"] = GetCurrentStatus();

  return config;
}

}
