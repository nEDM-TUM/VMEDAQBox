#ifndef _Firmware_hh_
#define _Firmware_hh_
#include "CFirmwareLib.h"
namespace cascade {
  void Print(const CFConfig& conf);
  void Print(const CFDiagnostics& conf);
  void Print(const CFStatistics& conf);
  void Print(const CFSimulation& conf);
}
#endif
