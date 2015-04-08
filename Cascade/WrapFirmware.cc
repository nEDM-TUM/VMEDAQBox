#include <boost/fusion/include/adapt_struct.hpp>
#include "CFirmwareLib.h" 
#include <iostream>

typedef DWORD DWORD_Four[4];
typedef DWORD DWORD_Five[5];
typedef DWORD DWORD_Ten[10];
typedef DWORD DWORD_Sixteen[16];
typedef DWORD SimPattern[NO_OF_CIPIX_PATTERNS];
typedef DWORD SimPatternFour[NO_OF_CIPIX_PATTERNS][4];
typedef std::string* std_string_p;

BOOST_FUSION_ADAPT_STRUCT(
  CFConfig,
   (DWORD_Five, dwDelayDemux)
   (DWORD_Ten, dwTMultiplexer)
   (DWORD_Four, dwMaskX)
   (DWORD_Four, dwMaskY)
   (DWORD, dwMaskT)
   (DWORD, dwTypeOfListModeData)
   (DWORD, dwEventFilterConfig)
   (DWORD, dwMaxEventSize)
   (DWORD, dwMaxEventLength)
   (DWORD_Sixteen, dwMaskAdr)
   //(std_string_p, pAdrMaskList)
   (DWORD_Four, dwTThreshold)
   (bool, bExternalTimeBinMode)
   (bool, bSlaveMode)
   (bool, bSingleSweepMode)
   (DWORD, dwSingleSweepSize)
   (DWORD, dwExternalCounterStop)
   (DWORD_Four, dwInputMultiplexer)
   (DWORD_Four, dwOutputMultiplexer)
   (DWORD, dwLengthTOFOut)
)


BOOST_FUSION_ADAPT_STRUCT(
  CFSimulation,
   (bool, bSimulationCIPix)
   (DWORD, dwNoOfPauseCycles)
   (DWORD, dwNoOfSimulationPatterns)
   (SimPatternFour, dwXSimulationPattern)
   (SimPatternFour, dwYSimulationPattern)
   (SimPattern, dwTSimulationPattern)
   (bool, bSimulationChopper)
   (DWORD, dwChopperPeriod)
   (bool, bMiezeTimeBinSimulation)
)

BOOST_FUSION_ADAPT_STRUCT(
  CFStatistics,
   (DWORD, dwSingle)
   (DWORD, dwDouble)
   (DWORD, dwTriple)
   (DWORD, dwMultiple)
   (DWORD, dwMultiHit)
   (DWORD, dwInvalid)
   (DWORD, dwTime1)
   (DWORD, dwTime2)
   (DWORD, dwTime3)
   (DWORD, dwTimeMultiple)
   (DWORD, dwTimeInvalid)
   (DWORD, dwSingleY)
   (DWORD, dwDoubleY)
   (DWORD, dwTripleY)
   (DWORD, dwMultipleY)
   (DWORD, dwMultiHitY)
   (DWORD, dwInvalidY)
   (DWORD, dwTime1Y)
   (DWORD, dwTime2Y)
   (DWORD, dwTime3Y)
   (DWORD, dwTimeMultipleY)
   (DWORD, dwTimeInvalidY)
   (DWORD, dwEMICorner)
   (DWORD, dwEMIAll)
   (DWORD, dwAmbigous)
   (DWORD, dwNoXEvent)
   (DWORD, dwNoYEvent)
   (DWORD, dwDephased)
   (DWORD, dwPure2D)
   (DWORD, dwFull)
   (DWORD, dwBroken)
   (DWORD, dwGemTop)
   (DWORD, dwGemBottom)
   (DWORD, dwMissingGem)
   (DWORD, dwMultiGem)
   (DWORD, dwCoupledGem)
)

BOOST_FUSION_ADAPT_STRUCT(
  CFDiagnostics,
  (DWORD, dwDemuxAsynchronCounter)
  (DWORD, dwDemuxAsynchronPattern)
  (DWORD, dwDemuxTimeOutFWCounter)
  (DWORD, dwDepthDataPipe)
  (ULONGLONG, ullEBNoOfMeasuredEvents)
  (DWORD, dwEBDataLostCounter)
  (DWORD, dwEDDataLostCounter)
  (DWORD, dwHBDataLostCounter)
)


