#include "BuildLambdaFunction.hh"
#include "WrapFirmware.hh"
#include "WrapBoard.hh"
#include "universe_api.h"
#include "TUniverseCDAQBox.hh"
#include "Configuration.hh"
#include <boost/bind.hpp>
#include "autobahn/autobahn.hpp"
#include <iostream>
#include <vector>


namespace cascade {
  class daqbox_error: public std::runtime_error
  {
    public:
      daqbox_error(const std::string& msg) : std::runtime_error(msg) {}
  };

}


struct MeasurementData {
    std::string   firmwareVersion;
    ULONGLONG     lifeTimeElapsed;
    ULONGLONG     eventsCounted;
    ULONGLONG     sweepsElapsed;
    double        chopperFrequency;
    CFConfig      firmwareConfig;
    HSetup        hardwareConfig;
    MConfig       measurementConfig;
    CFSimulation  simulationConfig;
    CFStatistics  statistics;
    CFDiagnostics diagnostics;
};

BOOST_FUSION_ADAPT_STRUCT(
  MeasurementData,
   (std::string  , firmwareVersion)
   (ULONGLONG    , lifeTimeElapsed)
   (ULONGLONG    , eventsCounted)
   (ULONGLONG    , sweepsElapsed)
   (double       , chopperFrequency)
   (CFConfig     , firmwareConfig)
   (HSetup       , hardwareConfig)
   (MConfig      , measurementConfig)
   (CFSimulation , simulationConfig)
   (CFStatistics , statistics)
   (CFDiagnostics, diagnostics)
)

typedef std::vector<DWORD> vec_type;

struct BinSizes {
    DWORD time;
    DWORD x;
    DWORD y;
};

BOOST_FUSION_ADAPT_STRUCT(
  BinSizes,
   (DWORD, time)
   (DWORD, x)
   (DWORD, y)
)

struct CurrentStatus {
    vec_type  data;
    BinSizes  bins;
    bool      isMeasuring;
    bool      isLocked;
    ULONGLONG absTimeElapsed;
    void Reset() { data.resize(bins.x*bins.y*bins.time); absTimeElapsed = 0;}
};

BOOST_FUSION_ADAPT_STRUCT(
  CurrentStatus,
   (vec_type , data)
   (BinSizes , bins)
   (bool     , isMeasuring)
   (bool     , isLocked)
   (ULONGLONG, absTimeElapsed)
)

namespace {

using cascade::detail::anyvec;
using cascade::detail::anymap;

typedef std::map<std::string, boost::any> check_return_map;
check_return_map gReturnMap;

template<typename T>
void RegisterTypeCheck(std::function<T(const T&)> func)
{
  gReturnMap[typeid(T).name()] = func;
}

template<typename T>
T PerformCheck(const T& val)
{
  auto check = gReturnMap.find(typeid(T).name());
  if (check == gReturnMap.end()) return val;
  return boost::any_cast<std::function<T(const T&)> >(check->second)(val);
}


TUniverseCDAQBox gDAQBox;

// Structs to hold status and measurement
CurrentStatus gStatus;
MeasurementData gMeasurementData;


template<class classType, typename funcType, typename F, typename... Args>
struct WrapFunction
{
  typedef std::function<F(Args...)> return_type;
  static return_type Wrap(classType& anobj, funcType func)
  {
    return [&anobj,func] (Args... args) {
      return PerformCheck((anobj.*func)(args...));
    };
  }
};

template <typename F>
struct WrapImpl;

// Handle Class member function
template<class C, typename F, typename... Args>
struct WrapImpl<F(C::*)(Args...)> : WrapFunction<C,F(C::*)(Args...),F,Args...>
{};

// Handle const class member function
template<class C, typename F, typename... Args>
struct WrapImpl<F(C::*)(Args...) const> : WrapFunction<C,F(C::*)(Args...) const,F,Args...>
{};

template<class C, typename F>
typename WrapImpl<F>::return_type Wrap(C& anobj, F func)
{
  return WrapImpl<F>::Wrap(anobj, func);
}

class InitializeDevice {
  public:
  InitializeDevice()
  {
    set_hw_byte_swap(true);
    // get version of the used HardwareLib and for wich OS it is compiled for
    std::cout << gDAQBox.GetVersionHardwareLib() << std::endl;
    std::cout << gDAQBox.GetVersionOS() << std::endl;

    // init the USB interface of the DAQBox
    std::cout << "0x" << std::hex << ( PerformCheck(gDAQBox.Init( 0x400000 )) )
              << " : DAQBox Init" << std::endl;
    gMeasurementData.firmwareVersion = gDAQBox.GetFirmwareVersion();
    using std::cout;
    using std::endl;
    using std::hex;
    // reset DAQBox, if wanted or needed
    cout << "0x" << std::hex << gDAQBox.Reset() << " : DAQBox Reset" << endl;

    // get firmware version of DAQBox
    cout << gDAQBox.GetFirmwareVersion() << endl;

    // get status register
    DWORD status;
    cout << "0x" << std::hex << gDAQBox.GetStatusRegister(&status) << " : DAQBox get status" << endl;
    cout << "0x" << std::hex << status << endl << endl;

    TUniverseCDAQBox* p_cf = &gDAQBox;
    TUniverseCDAQBox* p_m = &gDAQBox;

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

    cout << hex << p_cf->ConfigureFirmware( cconfig ) << " cf config" << endl;

    MConfig config = p_m->GetActualConfigurationOfMeasurement();

    // config of the measurement type
    config.dwTypeOfReadout = TWO_D_CROSSED_STRIPES;
    config.dwTypeOfMeasurement = IMAGE;
//  config.dwTypeOfMeasurement = TOF;
    config.dwXResolution = 128;
    config.dwYResolution = 128;

    // measurement time in terms of 100ns
    config.ullTimeToCount = 1*10000000; // 1 sec

    // prameters for TOF
    // dwelltime in terms of 100ns
    config.dwDwellTime = 10000; // 1 msec
    config.dwNoOfTimeBins = 1;
    config.bBreakMode = true;

    cout << hex << p_m->ConfigureMeasurement( config ) << " m config" << endl;



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

    cout << hex << p_cf->ConfigureFirmwareSimulation( simulation ) << " cf sim" << endl;
  }
};

InitializeDevice gDev;

// Steps of measurement
// 1. Configure Firmware
// 2. Configure Measurement
// 3. Configure simulation (if it is a simulation)
// 4. Generate buffer, read out
// At the end, readout data
boost::any StartMeasurement()
{
  // Check if we're running
  if (gStatus.isLocked || gStatus.isMeasuring) {
    throw cascade::daqbox_error("Card locked or running");
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

boost::any GetCurrentStatus()
{
  PerformCheck(gDAQBox.Measurement());
  gStatus.absTimeElapsed = gDAQBox.GetAbsTimeElapsed();
  DWORD status;
  PerformCheck(gDAQBox.GetStatusRegister(&status));
  gStatus.isMeasuring = ((status & 0x8) == 0);
  return cascade::EncAll<CurrentStatus>::encode(gStatus);
}

boost::any StopMeasurement()
{
  // Check if we're running
  if (!gStatus.isLocked) {
    throw cascade::daqbox_error("Card should be locked when calling stop measurement");
  }

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

  anymap config = boost::any_cast<anymap>(cascade::EncAll<MeasurementData>::encode(gMeasurementData));
  if (!gMeasurementData.simulationConfig.bSimulationCIPix) {
    // Remove from the config
    config.erase("simulationConfig");
  }
  config["status"] = GetCurrentStatus();

  gStatus.isLocked = false;
  return config;
}


}

// Interface
// Set/Get configuration
// Start/Stop Measurement
// Get Current Status

namespace cascade {
void define_daqbox_interface(cascade::session_type& s)
{

  RegisterTypeCheck<DWORD>([] (const DWORD& at) {
    if (at != EC_OK) {
      throw cascade::daqbox_error(gDAQBox.GetErrorText(at));
    }
    return at;
  });
  #define NOWRAP(func) \
  auto r1_ ## func = s.provide(#func, cascade::GenFunction(Wrap(gDAQBox, &TUniverseCDAQBox::func))); \
    r1_ ## func.then([](boost::future<autobahn::registration> reg) {\
       std::cerr << "procedure " #func "() registered with registration ID " << reg.get().id << std::endl;\
    }).wait();

  #define NOOBJWRAP(func) \
  auto r1_ ## func = s.provide(#func, cascade::GenFunction(&func)); \
    r1_ ## func.then([](boost::future<autobahn::registration> reg) {\
       std::cerr << "procedure " #func "() registered with registration ID " << reg.get().id << std::endl;\
    }).wait();

   //NOWRAP(GetFirmwareVersReg)
   //NOWRAP(GetSerialNumber)
   //.def<BL::strMemFn>("GetFirmwareVersion", &BL::GetFirmwareVersion )
   //.def<BL::strMemFn>("GetFirmwareSetup", &BL::GetFirmwareVersion )
   //NOWRAP(GetFirmwareSetupReg)
   //    // setup of the CFBoard hardware
   NOWRAP(GetActualHardwareSetup)
   NOWRAP(SetHardwareSetup)
   //    // configuration of the firmware
   NOWRAP(GetActualConfigurationOfFirmware)
   NOWRAP(ConfigureFirmware)
   //    // configuration of the simulation
   NOWRAP(GetActualConfigurationOfFirmwareSimulation)
   NOWRAP(ConfigureFirmwareSimulation)
   //    // configuration of the measurement
   NOWRAP(GetActualConfigurationOfMeasurement)
   NOWRAP(ConfigureMeasurement)

   NOOBJWRAP(StartMeasurement)
   NOOBJWRAP(StopMeasurement)
   NOOBJWRAP(GetCurrentStatus)
   //    //virtual DWORD InitDataBuffer( DWORD *i_pDataBuffer );
   //    //DWORD SetDataBuffer( DWORD *i_pDataBuffer );

   //    // actual status of firmware and hardware
   //NOWRAP(GetStatusRegister)

   //    //std::string GetStatusErrorText( const DWORD ErrorCode );
   //    // start, stop and measurement
   //NOWRAP(StartMeasurement)
   //NOWRAP(Measurement)
   //NOWRAP(GetListModeData)
   //NOWRAP(StopMeasurement)
   //NOWRAP(StopTOFMeasurementImmediately)
   //NOWRAP(HasMeasurementStopped)
   //NOWRAP(GetEntireMeasurement)
   //    // actual status of measurement

   //    // read from Event Data FIFO
   //    //DWORD BlockReadEDFifo( DWORD *pData, DWORD ReqNoOfDWords );

   //    // read from List Mode Data FIFO

   //    //DWORD BlockReadLMFifo( DWORD *pData, DWORD ReqNoOfDWords, const DWORD OffsetAdr );

   //    //  read from / write to SRAM

   //    //DWORD SingleReadSRAM( const DWORD SRAMAdr, DWORD *pData );
   //    //DWORD AutoIncrReadSRAM( DWORD *pData );
   //    //virtual DWORD BlockReadSRAM( DWORD *pData, DWORD ReqNoOfDWords, const WORD OffsetAdr = 0x00 );
   //NOWRAP(ClearSRAM)
   //NOWRAP(TestAllSRAM)
   //NOWRAP(TestSingleSRAM)
    ;
}
}
