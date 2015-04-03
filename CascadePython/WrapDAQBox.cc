#include <boost/python.hpp>
#include <boost/python/str.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
using namespace boost::python;

#include "DAQBoxLib.h"

class DAQBoxLib : public CDAQBoxLib
{
  public:
    typedef std::string (DAQBoxLib::*strMemFn)() const;
};
typedef DAQBoxLib BL;



void define_daqbox()
{
  #define NOWRAP(func) \
    .def(#func, &BL::func)
  PyEval_InitThreads();
  boost::python::class_<BL>("DAQBoxLib", init<>()) 
    NOWRAP(GetFirmwareVersReg)
    NOWRAP(GetSerialNumber)
    .def<BL::strMemFn>("GetFirmwareVersion", &BL::GetFirmwareVersion ) 
    .def<BL::strMemFn>("GetFirmwareSetup", &BL::GetFirmwareVersion ) 
    NOWRAP(GetFirmwareSetupReg)
	// setup of the CFBoard hardware
    NOWRAP(GetActualHardwareSetup)
    NOWRAP(SetHardwareSetup)
	// configuration of the firmware
    NOWRAP(GetActualConfigurationOfFirmware)
    NOWRAP(ConfigureFirmware)
    NOWRAP(GetActualConfigurationOfFirmwareSimulation)
    NOWRAP(ConfigureFirmwareSimulation)
	// configuration of the measurement
    NOWRAP(GetActualConfigurationOfMeasurement)
    NOWRAP(ConfigureMeasurement)

	//virtual DWORD InitDataBuffer( DWORD *i_pDataBuffer );
	//DWORD SetDataBuffer( DWORD *i_pDataBuffer );

	// actual status of firmware and hardware
    NOWRAP(GetStatusRegister)

	//std::string GetStatusErrorText( const DWORD ErrorCode );
	// start, stop and measurement
    NOWRAP(StartMeasurement)
    NOWRAP(Measurement)
    NOWRAP(GetListModeData)
    NOWRAP(StopMeasurement)
    NOWRAP(StopTOFMeasurementImmediately)
    NOWRAP(HasMeasurementStopped)
    NOWRAP(GetEntireMeasurement)
	// actual status of measurement
    NOWRAP(ReadStatistics)
    NOWRAP(ReadDiagnostics)
    NOWRAP(GetExternalCounter)
	// get measured data
    NOWRAP(GetAbsTimeElapsed)
    NOWRAP(GetLifeTimeElapsed)
    NOWRAP(GetEventsCounted)
    NOWRAP(GetSweepsElapsed)
    NOWRAP(GetChopperFrequency)

	// read from Event Data FIFO
	//DWORD BlockReadEDFifo( DWORD *pData, DWORD ReqNoOfDWords );

	// read from List Mode Data FIFO

	//DWORD BlockReadLMFifo( DWORD *pData, DWORD ReqNoOfDWords, const DWORD OffsetAdr );

	//  read from / write to SRAM

	//DWORD SingleReadSRAM( const DWORD SRAMAdr, DWORD *pData );
	//DWORD AutoIncrReadSRAM( DWORD *pData );
	//virtual DWORD BlockReadSRAM( DWORD *pData, DWORD ReqNoOfDWords, const WORD OffsetAdr = 0x00 );
    NOWRAP(ClearSRAM)
    NOWRAP(TestAllSRAM)
    NOWRAP(TestSingleSRAM)
    ;
}
