#include "BuildLambdaFunction.hh" 
#include "WrapFirmware.hh"
#include "WrapBoard.hh"
#include "TUniverseCDAQBox.hh"
#include "Configuration.hh"
#include <boost/bind.hpp>
#include "autobahn/autobahn.hpp"
#include <iostream>

namespace cascade {
void define_daqbox_interface(TUniverseCDAQBox& abox, cascade::session_type& s)
{
  #define NOWRAP(func, args...) \
  auto r1_ ## func = s.provide(#func, cascade::GenFunction(abox, &TUniverseCDAQBox::func, ##args)); \
    r1_ ## func.then([](boost::future<autobahn::registration> reg) {\
       std::cerr << "procedure " #func "() registered with registration ID " << reg.get().id << std::endl;\
    }).wait();

   NOWRAP(GetFirmwareVersReg)
   NOWRAP(GetSerialNumber)
   //.def<BL::strMemFn>("GetFirmwareVersion", &BL::GetFirmwareVersion ) 
   //.def<BL::strMemFn>("GetFirmwareSetup", &BL::GetFirmwareVersion ) 
   NOWRAP(GetFirmwareSetupReg)
   //    // setup of the CFBoard hardware
   NOWRAP(GetActualHardwareSetup)
   NOWRAP(SetHardwareSetup, _1)
   //    // configuration of the firmware
   NOWRAP(GetActualConfigurationOfFirmware)
   NOWRAP(ConfigureFirmware, _1)
   NOWRAP(GetActualConfigurationOfFirmwareSimulation)
   NOWRAP(ConfigureFirmwareSimulation, _1)
   //    // configuration of the measurement
   NOWRAP(GetActualConfigurationOfMeasurement)
   NOWRAP(ConfigureMeasurement, _1)

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
