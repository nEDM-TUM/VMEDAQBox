#include <iostream>
#include <vector>
#include "TUniverseCDAQBox.hh"
#include "universe_api.h"
#include "CDRLib.h"
using namespace std;

int main()
{
    //------------------------- CASCADE DAQBox ----------------------------------------------------
    

    set_hw_byte_swap(true);

    TUVMEDevice* dev = get_new_device(0x0,             // Base VME address
                            0x39,// DL710 supports only A24, D32  
                            0x4,  // DL710 supports only A24, D32  
                            0x0   // Gets maximum sized image 
                            );


    uint32_t readNum = 0x20000;
    uint32_t readIn;
    //for (uint32_t i=0;i<readNum;i++) {
    //  dev->Read((char*)&readIn, sizeof(readIn), 0x400960);
    //  cout << "0x" << hex << readIn << endl;
    //}

    cout << "Break" << endl;
    vector<uint32_t> new_vec(readNum);
    dev = get_dma_device(0x400960, 0x39, 0x4, false);
    dev->Read((char*)&new_vec[0], readNum*4, 0x0);
    for (uint32_t i=0;i<readNum;i++) {
      cout << "0x" << hex << new_vec[i] << endl;
    }

    return 0;

}
