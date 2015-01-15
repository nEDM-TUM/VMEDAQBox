#include <iostream>
#include "TUniverseCBusLib.hh"
#include "TUniverseCDAQBox.hh"
#include "CDRLib.h"
using namespace std;

int main()
{
	//------------------------- CASCADE DAQBox ----------------------------------------------------
	


	TUniverseCDAQBox box;

	// get version of the used HardwareLib and for wich OS it is compiled for
	cout << box.GetVersionHardwareLib() << endl;
	cout << box.GetVersionOS() << endl << endl;

	// init the USB interface of the DAQBox
	DWORD time_out = 2000, error;
	string serial_number = "BPDUSEC1JP";
	cout << "0x" << hex << ( error = box.Init( serial_number, time_out ) ) << " : DAQBox Init" << endl;

        CFConfig conf = box.GetActualConfigurationOfFirmware();
        cout << "0x" << hex << box.ConfigureFirmware(conf) << endl;

	cout << "Firmware: 0x" << hex << box.GetFirmwareVersReg() << endl;
	cout << "Firmware: 0x" << hex << box.GetFirmwareSetupReg() << endl;
	// If any error code unequal 0 (= EC_OK) has been returned by any method of any object 
	// of the HardwareLib, the meaning and (until some extend) the history of the error propagation
	// inside the code can be obtained in text-form by using the method GetErrorText() of this object!
	if ( error )
	{
		cout << box.GetErrorText( error ) << endl;
		cout << box.GetSerialNumber() << endl;
	}

	// reset DAQBox, if wanted or needed
	cout << "0x" << hex << box.Reset() << " : DAQBox Reset" << endl;

	// get firmware version of DAQBox 
	cout << box.GetFirmwareVersion() << endl;

	// get status register
	DWORD status;
	cout << "0x" << hex << box.GetStatusRegister(&status) << " : DAQBox get status" << endl;
	cout << "0x" << hex << status << endl << endl;


	//--------------------- Block RAM Transfer ----------------------------------------------------

	return 0;

}
