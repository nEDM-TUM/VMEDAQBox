#include "WrapFirmware.hh"
#include "WrapBoard.hh"
#include "MeasurementStruct.hh"
#include "StructConversion.hh"
#include "PrintStruct.hh"

BOOST_FUSION_ADAPT_STRUCT(
  MeasurementData,
   (std::string  , firmwareVersion)
   (ULONGLONG    , lifeTimeElapsed)
   (ULONGLONG    , eventsCounted)
   (ULONGLONG    , sweepsElapsed)
   (double       , chopperFrequency)
   (CFConfig     , firmwareConfig)
   (HSetup       , hardwareConfig)
   (MConfig      , measurementConfig)
   (CFSimulation , simulationConfig)
   (CFStatistics , statistics)
   (CFDiagnostics, diagnostics)
)

BOOST_FUSION_ADAPT_STRUCT(
  BinSizes,
   (DWORD, time)
   (DWORD, x)
   (DWORD, y)
)

BOOST_FUSION_ADAPT_STRUCT(
  CurrentStatus,
   (vec_type , data)
   (BinSizes , bins)
   (bool     , isMeasuring)
   (ULONGLONG, absTimeElapsed)
)

boost::any MeasurementData::encode()
{
  return cascade::EncAll<MeasurementData>::encode(*this);
}

boost::any CurrentStatus::encode()
{
  return cascade::EncAll<CurrentStatus>::encode(*this);
}

void CurrentStatus::Reset()
{
  data.resize(bins.x*bins.y*bins.time);
  absTimeElapsed = 0;
  memset(&data[0], 0, data.size()*sizeof(data[0]));
}
