#include "TUniverseCDAQBox.hh"
#include "universe_api.h"
#include <exception>


const int32_t DL710_DataSize = 4;
const uint32_t DL710_AddressMod = 0x39;

#define CallParentFunction(func) \
  DWORD retVal = CDAQBox::func();\
  if (retVal != EC_OK) return retVal;

class DeviceException : public std::exception
{
  public:
    const char* what() const throw()
    {
      return "VME device is unavailable";
    }
} loc1_Exception;

TUniverseCDAQBox::TUniverseCDAQBox() : CDAQBoxLib(), m_VMEDev(0), m_ControlDevice(0)
{
  // Map the whole A24 space to this device.  If it can't be opened, throw an
  // exception. 
  return;
  m_VMEDev = get_new_device(0x0,             // Base VME address
                            DL710_AddressMod,// DL710 supports only A24, D32  
                            DL710_DataSize,  // DL710 supports only A24, D32  
                            0x0   // Gets maximum sized image 
                            );
  if (!m_VMEDev) throw loc1_Exception;
  m_ControlDevice = get_ctl_device();
  if (!m_ControlDevice) throw loc1_Exception;
}

TUniverseCDAQBox::~TUniverseCDAQBox()
{
  Close(); 
}

DWORD TUniverseCDAQBox::Init(const std::string i_SB, const DWORD waitTime)
{
  
  return EC_OK;
}

DWORD TUniverseCDAQBox::Close()
{
  // Close the VME device if it's open.
  //CallParentFunction(Close)
  if (m_VMEDev) close_device(m_VMEDev);
  m_VMEDev = 0;
  return EC_OK;
}

DWORD TUniverseCDAQBox::Reset()
{
  //CallParentFunction(Reset)
  return EC_OK;
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

DWORD TUniverseCDAQBox::ReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, DWORD *pData )
{
  return EC_OK;
  if (!m_VMEDev || 
       m_VMEDev->Read((char*)pData, DL710_DataSize, (uint32_t) SubModuleAddr + offset) != DL710_DataSize) { 
    return ES_DeviceDriver; 
  }
  return EC_OK;
}

DWORD TUniverseCDAQBox::WriteDWordSubModule( const WORD SubModuleAddr, const WORD offset, DWORD pData )
{
  return EC_OK;
  if (!m_VMEDev || 
       m_VMEDev->Write((char*)&pData, DL710_DataSize, (uint32_t) SubModuleAddr + offset) != DL710_DataSize) { 
    return ES_DeviceDriver; 
  }
  return EC_OK;
}

DWORD TUniverseCDAQBox::DMAReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, 
                             const DWORD ReqNoOfDWords, DWORD *pData )
{
  // DMA read
  return EC_OK;
  TUVMEDevice* dev = get_dma_device(0x0, DL710_AddressMod, DL710_DataSize, false);
  if (!dev) return ES_DeviceDriver;
  int32_t retVal = dev->Read((char*)pData, (uint32_t)ReqNoOfDWords*DL710_DataSize, (uint32_t)SubModuleAddr + offset)/DL710_DataSize;
  if (retVal != ReqNoOfDWords) {
    return ES_DeviceDriver;
  }
  return EC_OK; 
}

bool TUniverseCDAQBox::GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName )
{
  if (!pClassAndMethodName) return false;
  *pClassAndMethodName = "TUniverseCDAQBox::" + GetMethodName(MethodId);
  return true;
}


