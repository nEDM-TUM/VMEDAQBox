#include "DefineStruct.hh"
#include <boost/fusion/include/adapt_struct.hpp>
#include "MBoardLib.h" 

typedef DWORD ChannelArray[64];

BOOST_FUSION_ADAPT_STRUCT(
  HSetup,
   (DWORD, dwTypeOfReadoutStructure)
   (DWORD, dwNoOfXStripesOrPixel)
   (DWORD, dwNoOfYStripesOrPixel)
   (DWORD, dwMeasurementCapabilityOfHardware)
   (DWORD, dwMinTimeToCount)
   (ULONGLONG, ullMaxTimeToCount)
   (DWORD, dwMinDwellTime)
   (DWORD, dwMaxDwellTime)
   (DWORD, dwMaxNoOfOffsetBins)
   (DWORD, dwMinNoOfTimeBins)
   (DWORD, dwMaxNoOfTimeBins)
   (ULONGLONG, ullMaxNoOfSweeps)
   (bool, bBreakModePossible)
   (DWORD, dwMaxMemory)
   (bool, bSelectionOfChannelsPossible)
   (bool, bListModePossible)
   (bool, bExternalCounterPossible)
)

BOOST_FUSION_ADAPT_STRUCT(
  ChannelList,
   (DWORD, size)
   (ChannelArray, x)
   (ChannelArray, y)
)

BOOST_FUSION_ADAPT_STRUCT(
  MConfig,
   (DWORD, dwTypeOfReadout)
   (DWORD, dwXResolution)
   (DWORD, dwYResolution)
   (DWORD, dwTypeOfMeasurement)
   (ULONGLONG, ullTimeToCount)
   (DWORD, dwDwellTime)
   (DWORD, dwNoOfOffsetBins)
   (DWORD, dwNoOfTimeBins)
   (ULONGLONG, ullNoOfSweeps)
   (bool, bBreakMode)
   (bool, bSelectionOfChannels)
   (ChannelList, ListOfSelectedChannels)
   (bool, bListMode)
   //(std::ofstream *pListModeFile)
   (bool, bListModeFileBinary)
)

void define_board()
{
  define_struct<HSetup>("h_setup");
  define_struct<ChannelList>("channel_list");
  define_struct<MConfig>("m_config");
  ToPython<ChannelArray>();
};
