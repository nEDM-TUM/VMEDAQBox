#ifndef _TUniverseCDAQBox_hh_
#define _TUniverseCDAQBox_hh_

#include "DAQBoxLib.h"

class TUVMEDevice;

class TUniverseCDAQBox : public CDAQBoxLib
{

  public:
    TUniverseCDAQBox();
    virtual ~TUniverseCDAQBox();

    // init/close/reset of the VME bus
    enum EFunctions {
      EC_UniverseCDAQBox =  0x27000000, // Class ID
      kInit              =  EC_UniverseCDAQBox | 0x10000, // Method ID
      kClose             =  EC_UniverseCDAQBox | 0x20000,
      kReset             =  EC_UniverseCDAQBox | 0x30000,
      kReadDWordSubModule              =  EC_UniverseCDAQBox | 0x40000,
      kWriteDWordSubModule             =  EC_UniverseCDAQBox | 0x50000,
      kDMAReadDWordSubModule           =  EC_UniverseCDAQBox | 0x60000
    };

    DWORD Init( DWORD vmeBaseAdress );
    virtual DWORD Close();
    virtual DWORD Reset();
    virtual DWORD StopMeasurement();

    // communication with the remote station of the interface

    virtual DWORD ReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, DWORD *pData );
    virtual DWORD WriteDWordSubModule( const WORD SubModuleAddr, const WORD offset, const DWORD pData );
    virtual DWORD DMAReadDWordSubModule( const WORD SubModuleAddr, const WORD offset,
                                             const DWORD ReqNoOfDWords, DWORD *pData );

    virtual bool GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName );

  protected:
    DWORD CallSysReset();
    DWORD TranslateAddress( DWORD SubModuleAddr, DWORD offset) const;
    std::string GetMethodName( const DWORD MethodId );

  private:
    TUVMEDevice *m_VMEDev;
    TUVMEDevice *m_ControlDevice;
    DWORD        m_BaseAddress;


	// Unfortunately, these were made virtual and are *hidden* in the
	// inheritance tree.  We explicitly *do not* implement them here.
    DWORD Init( const std::string, const DWORD ) { return 0; }
    DWORD Init( CBusLib *, const DWORD ) { return 0; }

    // Disable copying
    TUniverseCDAQBox(const TUniverseCDAQBox&);
    TUniverseCDAQBox& operator=(const TUniverseCDAQBox&);


};

#endif /* TUniverseCDAQBox_hh */

