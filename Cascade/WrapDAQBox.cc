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

namespace {


struct MeasurementData {
    std::string   firmwareVersion;
    ULONGLONG     absTimeElapsed;
    ULONGLONG     lifeTimeElapsed;
    ULONGLONG     eventsCounted;
    ULONGLONG     sweepsElapsed;
    double        chopperFrequency;
    CFConfig      firmwareConfig;
    HSetup        hardwareConfig;
    MConfig       measurementConfig;
    CFStatistics  statistics;
    CFDiagnostics diagnostics;
};

typedef std::vector<DWORD> vec_type;

struct BinSizes {
    DWORD time;
    DWORD x;
    DWORD y;
};

struct CurrentStatus {
    vec_type  data;
    BinSizes  bins;
    bool      isMeasuring;
    bool      isLocked;
};

// Steps of measurement
// 1. Configure Firmware
// 2. Configure Measurement
// 3. Configure simulation (if it is a simulation)
// 4. Generate buffer, read out 
// At the end, readout data

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
      return (anobj.*func)(args...);
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
    DWORD error;
    std::cout << "0x" << std::hex << ( error = gDAQBox.Init( 0x400000 ) ) 
              << " : DAQBox Init" << std::endl;
  }
};

InitializeDevice gDev;  

}
/*
                  any t = cascade::EncAll<CFConfig>::encode(CFConfig());
                  cerr << t.type().name() << endl;
                  session.call("send_to_database", {t}) 
                  .then([&](future<any> f) {
                     try {
                        bool result = any_cast<bool> (f.get());
                        cerr << "s2d() called with result: " << result << endl;
                     } catch (...) {
                        cerr << "s2d() call failed" << endl;
                     }
                  }
                        );

*/

// Interface
// Set/Get configuration
// Start/Stop Measurement
// Get Current Status

namespace cascade {
void define_daqbox_interface(cascade::session_type& s)
{
  #define NOWRAP(func, args...) \
  auto r1_ ## func = s.provide(#func, cascade::GenFunction(Wrap(gDAQBox, &TUniverseCDAQBox::func), ##args)); \
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

   //    //virtual DWORD InitDataBuffer( DWORD *i_pDataBuffer );
   //    //DWORD SetDataBuffer( DWORD *i_pDataBuffer );

   //    // actual status of firmware and hardware
   //NOWRAP(GetStatusRegister)

   //    //std::string GetStatusErrorText( const DWORD ErrorCode );
   //    // start, stop and measurement
   NOWRAP(StartMeasurement)
   NOWRAP(Measurement)
   //NOWRAP(GetListModeData)
   NOWRAP(StopMeasurement)
   NOWRAP(StopTOFMeasurementImmediately)
   //NOWRAP(HasMeasurementStopped)
   NOWRAP(GetEntireMeasurement)
   //    // actual status of measurement
   //NOWRAP(ReadStatistics)
   //NOWRAP(ReadDiagnostics)
   NOWRAP(GetExternalCounter)
   // get measured data
   NOWRAP(GetAbsTimeElapsed)
   NOWRAP(GetLifeTimeElapsed)
   NOWRAP(GetEventsCounted)
   NOWRAP(GetSweepsElapsed)
   NOWRAP(GetChopperFrequency)

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
