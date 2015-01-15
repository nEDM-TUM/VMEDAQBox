#ifndef TUniverseCBusLib_hh
#define TUniverseCBusLib_hh

#include "BusLib.h"

class TUVMEDevice;

class TUniverseCBusLib : public CBusLib
{

  public:
	TUniverseCBusLib();
	virtual ~TUniverseCBusLib();

	// init/close/reset of the VME bus

	virtual DWORD Init( const DWORD i_InterfaceNo, const DWORD i_WaitBus,
						const std::string i_DriverPath = "" );
	virtual DWORD Close();
	virtual DWORD Reset();

	// communication with the remote station of the interface

	virtual DWORD RemoteRead( const DWORD RemoteAdr, DWORD *pData );
	virtual DWORD RemoteWrite( const DWORD RemoteAdr, const DWORD Data );

	// DMA read

	virtual DWORD DMAReadDWord( const DWORD BusAdr, const DWORD ReqNoOfDWords,
								DWORD *pGotNoOfDWords, DWORD *pData );
	// interrupt handling

	virtual DWORD EnableIRQ();
	virtual DWORD DisableIRQ();
	virtual bool GetClassAndMethod( const DWORD MethodId, std::string *pClassAndMethodName );

  private:
    TUVMEDevice *m_VMEDev;
    TUVMEDevice *m_ControlDevice;

    // Disable copying
    TUniverseCBusLib(const TUniverseCBusLib&);
    TUniverseCBusLib& operator=(const TUniverseCBusLib&);


};

#endif /* TUniverseCBusLib_hh */
