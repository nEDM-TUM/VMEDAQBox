#ifndef TUniverseCDAQBox_hh
#define TUniverseCDAQBox_hh

#include "DAQBoxLib.h"

class TUVMEDevice;

class TUniverseCDAQBox : public CDAQBoxLib
{

  public:
    TUniverseCDAQBox();
    virtual ~TUniverseCDAQBox();

    // init/close/reset of the VME bus

    DWORD Init( DWORD vmeBaseAdress );
    virtual DWORD Close();
    virtual DWORD Reset();

    // communication with the remote station of the interface

    virtual DWORD ReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, DWORD *pData );
    virtual DWORD WriteDWordSubModule( const WORD SubModuleAddr, const WORD offset, const DWORD pData );
    virtual DWORD DMAReadDWordSubModule( const WORD SubModuleAddr, const WORD offset,
                                             const DWORD ReqNoOfDWords, DWORD *pData );

    virtual bool GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName );

  protected:
    DWORD CallSysReset();
    DWORD TranslateAddress( DWORD SubModuleAddr, DWORD offset) const;

  private:
    TUVMEDevice *m_VMEDev;
    TUVMEDevice *m_ControlDevice;
    DWORD        m_BaseAddress;

     
	// Unfortunately, these were made virtual and are *hidden* in the
	// inheritance tree.  We explicitly *do not* implement them here.
	DWORD Init( const std::string, const DWORD );
	DWORD Init( CBusLib *, const DWORD );

    // Disable copying
    TUniverseCDAQBox(const TUniverseCDAQBox&);
    TUniverseCDAQBox& operator=(const TUniverseCDAQBox&);


};

#endif /* TUniverseCDAQBox_hh */

