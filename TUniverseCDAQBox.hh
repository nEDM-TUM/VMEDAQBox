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

	virtual DWORD Init( DWORD vmeBaseAdress );
	virtual DWORD Close();
	virtual DWORD Reset();

	// communication with the remote station of the interface

	virtual DWORD ReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, DWORD *pData );
	virtual DWORD WriteDWordSubModule( const WORD SubModuleAddr, const WORD offset, const DWORD pData );
	virtual DWORD DMAReadDWordSubModule( const WORD SubModuleAddr, const WORD offset, 
                                             const DWORD ReqNoOfDWords, DWORD *pData );

	virtual bool GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName );

  private:
    TUVMEDevice *m_VMEDev;
    TUVMEDevice *m_ControlDevice;
    DWORD        m_BaseAddress;

    // Disable copying
    TUniverseCDAQBox(const TUniverseCDAQBox&);
    TUniverseCDAQBox& operator=(const TUniverseCDAQBox&);


};

#endif /* TUniverseCDAQBox_hh */
