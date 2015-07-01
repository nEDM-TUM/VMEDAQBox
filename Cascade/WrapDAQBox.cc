#include "BuildLambdaFunction.hh"
#include "Configuration.hh"
#include <boost/bind.hpp>
#include "autobahn/autobahn.hpp"
#include <iostream>
#include "InitializeDevice.hh"
#include "ReturnCheck.hh"
#include "Error.hh"
#include "WrapBoard.hh"
#include "WrapFirmware.hh"

namespace {

using cascade::PerformCheck;


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

}


// Steps of measurement
// 1. Configure Firmware
// 2. Configure Measurement
// 3. Configure simulation (if it is a simulation)
// 4. Generate buffer, read out
// At the end, readout data


// Interface
// Set/Get configuration
// Start/Stop Measurement
// Get Current Status

namespace cascade {
void define_daqbox_interface(cascade::session_type& s)
{

  TUniverseCDAQBox& gDAQBox = InitializeDevice::Device()._DAQBox;
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
