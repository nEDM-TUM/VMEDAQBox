#include "TUniverseCDAQBox.hh"
#include "universe_api.h"
#include <exception>


namespace {
const int32_t DL710_DataSize = 4;
const int32_t DL710_Shift = 4;
const uint32_t DL710_AddressMod = 0x39;
const DWORD Version_Firmware = 0x33;
};

/*
 *
 * Thrown exception when VME devices are not available.
 * */
class DeviceException : public std::exception
{
  public:
    const char* what() const throw()
    {
      return "VME device is unavailable";
    }
} loc_Exception;

//_____________________________________________________________________________
TUniverseCDAQBox::TUniverseCDAQBox() :
    CDAQBoxLib(),
    m_VMEDev(0),
           m_ControlDevice(0)
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

  *pCDevice = "TUniverseCDAQBox";

  ActualSetup.dwMeasurementCapabilityOfHardware = IMAGE | TOF;
  ActualSetup.dwMaxNoOfOffsetBins = 0;
  ActualSetup.dwMaxNoOfTimeBins = (0x1 << 23) - 1;    // 21 bit - 1
  ActualSetup.dwMaxMemory = (0x1 << 23);              // 21 bit

}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::CallSysReset()
{
  // Call Sys Reset on VME Bus
  uint32_t readDev;
  if ( sizeof(readDev) !=
        m_ControlDevice->Read((char*)&readDev, sizeof(readDev), 0x404 ) ) {
    return ES_DeviceDriver;
  }
  readDev |= 0x404000;
  if ( sizeof(readDev) !=
        m_ControlDevice->Write((char*)&readDev, sizeof(readDev), 0x404 ) ) {
    return ES_DeviceDriver;
  }
  return EC_OK;
}

//_____________________________________________________________________________
TUniverseCDAQBox::~TUniverseCDAQBox()
{
  // Close on destructor
  Close();
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::Init(DWORD vmeBaseAddress)
{
  // Initialize, this just merely sets the VME base address.
  m_BaseAddress = vmeBaseAddress;

  // read out the firmware version and generate 'sFirmwareVersion'
  if ( DWORD error = GetFirmwareVersion( pFirmwareVersion, &dwFirmwareVersReg ) )
  {
      Close();
      return error;

  }

  // Test, if firmware is correct.
  if ( (dwFirmwareVersReg >> 24) != Version_Firmware )
  {
      pStream->str("");
      *pStream << "\nThis CASCADE Hardwarelib is optimized for firmware " << std::hex <<
                  ((Version_Firmware & 0xf0) >> 4) << "." << (Version_Firmware & 0xf) <<
                  "!" << std::endl;
      *pFirmwareVersion += pStream->str();

      Close();
      return kInit | EH_WrongFirmware;
  }

  HSetup h_setup;
  if ( DWORD error = GetFirmwareSetup( pFirmwareSetup, &dwFirmwareSetupReg, &h_setup  ) )
  {
      Close();
      return error;
  }

  if ( DWORD error = SetHardwareSetup( h_setup ) )
  {
      Close();
      return error;
  }
  bInitialized = true;
  return EC_OK;
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::Close()
{
  // Close the VME device if it's open.
  if (m_VMEDev) close_device(m_VMEDev);
  m_VMEDev = 0;
  return EC_OK;
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::Reset()
{
  // Reset the module
  WriteDWordSubModule(0xa, 0x0, 1);
  return Init(m_BaseAddress);
}


//_____________________________________________________________________________
DWORD TUniverseCDAQBox::TranslateAddress( DWORD SubModuleAddr,
        DWORD offset) const
{
  // Translate from 'DAQBox' space to VME space.
  return  m_BaseAddress + ((SubModuleAddr + offset) << DL710_Shift);
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::ReadDWordSubModule(
        const WORD SubModuleAddr,
        const WORD offset,
        DWORD *pData )
{
  // Read Sub-module at offset
  uint32_t address = TranslateAddress(SubModuleAddr, offset);
  if (!m_VMEDev ||
       m_VMEDev->Read((char*)pData, DL710_DataSize, address) != DL710_DataSize) {
    return kReadDWordSubModule | ES_DeviceDriver;
  }
  return EC_OK;
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::WriteDWordSubModule(
        const WORD SubModuleAddr,
        const WORD offset,
        DWORD pData )
{
  // Write Sub-module at offset
  uint32_t address = TranslateAddress(SubModuleAddr, offset);
  if (!m_VMEDev ||
       m_VMEDev->Write((char*)&pData, DL710_DataSize, address) != DL710_DataSize) {
    return kWriteDWordSubModule | ES_DeviceDriver;
  }
  return EC_OK;
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::DMAReadDWordSubModule(
        const WORD SubModuleAddr,
        const WORD offset,
        const DWORD ReqNoOfDWords,
        DWORD *pData )
{
  // DMA read
  // Sub-module at offset
  // ** Note ** this is a non-incrementing read!  That means, the same address
  // is polled ReqNoOfDWords times.
  //
  uint32_t address = TranslateAddress(SubModuleAddr, offset);

  // Grab a DMA device
  TUVMEDevice* dev =
    get_dma_device(address, DL710_AddressMod, DL710_DataSize, false);
  if (!dev) return kDMAReadDWordSubModule | ES_DeviceDriver;
  int32_t retVal = dev->Read((char*)pData,
                             (uint32_t)ReqNoOfDWords*DL710_DataSize,
                             0x0)/DL710_DataSize;
  // Be sure to release it back.
  release_dma_device();

  return ((uint32_t)retVal != ReqNoOfDWords) ? (kDMAReadDWordSubModule | ES_DeviceDriver) : EC_OK;
}

//_____________________________________________________________________________
DWORD TUniverseCDAQBox::StopMeasurement()
{
  return WriteDWordSubModule(0x60, 0x0, 0x2);
}

//_____________________________________________________________________________
bool TUniverseCDAQBox::GetClassAndMethod(
        const DWORD MethodId,
        std::string *pClassAndMethodName )
{
    bClassIdAlreadyChecked = true;
    if ( (MethodId & EC_Mask) == EC_UniverseCDAQBox )
    {
        *pClassAndMethodName = "TUniverseCDAQBox." + GetMethodName( MethodId ) + ": ";
        bClassIdAlreadyChecked = false;
        return true;
    }

    if ( CCFirmwareLib::GetClassAndMethod( MethodId, pClassAndMethodName ) )
    {
        *pClassAndMethodName = "TUniverseCDAQBox\n" + *pClassAndMethodName;
        return true;
    }

    return false;
}

//_____________________________________________________________________________
std::string TUniverseCDAQBox::GetMethodName( const DWORD MethodId )
{
    switch ( MethodId )
    {
        case kInit:                 return "Init()";
        case kReset:                return "Reset()";
        case kReadDWordSubModule:   return "ReadDWordSubModule()";
        case kWriteDWordSubModule:  return "WriteDWordSubModule()";
        case kDMAReadDWordSubModule:return "DMAReadDWordSubModule()";

        default: return CErrorLib::GetMethodName( MethodId );
    }
}


