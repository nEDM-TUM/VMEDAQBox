#include "TUniverseCBusLib.hh"
#include "universe_api.h"
#include <exception>


const int32_t DL710_DataSize = 4;
const uint32_t DL710_AddressMod = 0x39;

#define CallParentFunction(func) \
  DWORD retVal = CBusLib::func();\
  if (retVal != EC_OK) return retVal;

class DeviceException : public std::exception
{
  public:
    const char* what() const throw()
    {
      return "VME device is unavailable";
    }
} loc_Exception;

TUniverseCBusLib::TUniverseCBusLib()
{
  // Map the whole A24 space to this device.  If it can't be opened, throw an
  // exception. 
  m_VMEDev = get_new_device(0x0,             // Base VME address
                            DL710_AddressMod,// DL710 supports only A24, D32  
                            DL710_DataSize,  // DL710 supports only A24, D32  
                            0x0   // Gets maximum sized image 
                            );
  if (!m_VMEDev) throw loc_Exception;
  m_ControlDevice = get_ctl_device();
  if (!m_ControlDevice) throw loc_Exception;
}

TUniverseCBusLib::~TUniverseCBusLib()
{
  Close(); 
}

DWORD TUniverseCBusLib::Init( const DWORD i_InterfaceNo, const DWORD i_WaitBus,
                  			  const std::string i_DriverPath)
{
  DWORD retVal = CBusLib::Init(i_InterfaceNo, i_WaitBus, i_DriverPath);
  return retVal;
}

DWORD TUniverseCBusLib::Close()
{
  // Close the VME device if it's open.
  CallParentFunction(Close)
  if (m_VMEDev) close_device(m_VMEDev);
  m_VMEDev = 0;
  return EC_OK;
}

DWORD TUniverseCBusLib::Reset()
{
  CallParentFunction(Reset)
  uint32_t readDev;
  if ( sizeof(readDev) != m_ControlDevice->Read((char*)&readDev, sizeof(readDev), 0x404 ) ) {
    return ES_DeviceDriver;
  }
  readDev |= 0x404000;
  if ( sizeof(readDev) != m_ControlDevice->Write((char*)&readDev, sizeof(readDev), 0x404 ) ) {
    return ES_DeviceDriver;
  }
  
  return EC_OK;
}

	// communication with the remote station of the interface

DWORD TUniverseCBusLib::RemoteRead( const DWORD RemoteAdr, DWORD *pData )
{
  if (!m_VMEDev || 
       m_VMEDev->Read((char*)pData, DL710_DataSize, RemoteAdr) != DL710_DataSize) { 
    return ES_DeviceDriver; 
  }
  return EC_OK;
}

DWORD TUniverseCBusLib::RemoteWrite( const DWORD RemoteAdr, const DWORD Data )
{
  if (!m_VMEDev || 
       m_VMEDev->Write((char*)&Data, DL710_DataSize, RemoteAdr) != DL710_DataSize) { 
    return ES_DeviceDriver; 
  }
  return EC_OK;
}

DWORD TUniverseCBusLib::DMAReadDWord( const DWORD BusAdr, const DWORD ReqNoOfDWords,
					DWORD *pGotNoOfDWords, DWORD *pData )
{
  // DMA read
  TUVMEDevice* dev = get_dma_device(0x0, DL710_AddressMod, DL710_DataSize, false);
  if (!dev) return ES_DeviceDriver;
  int32_t retVal = dev->Read((char*)pData, (uint32_t)ReqNoOfDWords*DL710_DataSize, BusAdr)/DL710_DataSize;
  if (retVal < 0) {
    *pGotNoOfDWords = 0;
    return ES_DeviceDriver;
  }
  *pGotNoOfDWords = retVal;
  return EC_OK; 
}

DWORD TUniverseCBusLib::EnableIRQ()
{
  // Interrupts are disabled
  return ES_DeviceDriver;
}

DWORD TUniverseCBusLib::DisableIRQ()
{
  // Interrupts are disabled
  return ES_DeviceDriver;
}

bool TUniverseCBusLib::GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName )
{
  if (!pClassAndMethodName) return false;
  *pClassAndMethodName = "TUniverseCBusLib::" + GetMethodName(MethodId);
  return true;
}


