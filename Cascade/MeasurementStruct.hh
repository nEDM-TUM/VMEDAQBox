#ifndef _MeasurementStruct_hh_
#define _MeasurementStruct_hh_

#include <vector>
#include <boost/any.hpp>
#include "MBoardLib.h"
#include "CFirmwareLib.h"

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

    boost::any    encode();
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
    ULONGLONG absTimeElapsed;

    void          Reset() { data.resize(bins.x*bins.y*bins.time); absTimeElapsed = 0;}
    boost::any    encode();
};

#endif /*_MeasurementStruct_hh_ */

