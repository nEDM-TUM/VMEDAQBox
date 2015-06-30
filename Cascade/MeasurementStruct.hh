#ifndef _MeasurementStruct_hh_
#define _MeasurementStruct_hh_

#include "WrapFirmware.hh"
#include "WrapBoard.hh"
#include <vector>

struct MeasurementData {
    std::string   firmwareVersion;
    ULONGLONG     lifeTimeElapsed;
    ULONGLONG     eventsCounted;
    ULONGLONG     sweepsElapsed;
    double        chopperFrequency;
    CFConfig      firmwareConfig;
    HSetup        hardwareConfig;
    MConfig       measurementConfig;
    CFSimulation  simulationConfig;
    CFStatistics  statistics;
    CFDiagnostics diagnostics;
};

typedef std::vector<DWORD> vec_type;

struct BinSizes {
    DWORD time;
    DWORD x;
    DWORD y;
};

struct CurrentStatus {
    vec_type  data;
    BinSizes  bins;
    bool      isMeasuring;
    bool      isLocked;
    ULONGLONG absTimeElapsed;
    void Reset() { data.resize(bins.x*bins.y*bins.time); absTimeElapsed = 0;}
};

#endif /*_MeasurementStruct_hh_ */

