#include "DefineStruct.hh"
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

struct Wrap_CFConfig
{
  Wrap_CFConfig ()
  {
    boost::python::converter::registry::push_back(
          &convertible,
          &construct,
          boost::python::type_id<CFConfig>());
    boost::python::to_python_converter<CFConfig, Wrap_CFConfig >();
  }
  static void* convertible(PyObject* obj_ptr)
  {
    return FromPython<CFConfig>::convertible(obj_ptr);
  }

  static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    
    FromPython<CFConfig>::construct(obj_ptr, data);
    CFConfig* conf = (CFConfig*) data->convertible;
    boost::python::dict o = boost::python::extract<boost::python::dict>(obj_ptr);
    if (conf->pAdrMaskList) {
      *conf->pAdrMaskList = boost::python::extract<const char*>(o["pAdrMaskList"]);
    }
  }

  static PyObject* convert(const CFConfig& s)
  {
    boost::python::dict o = boost::python::extract<boost::python::dict>(EncAll<CFConfig>::encode(s));
    if (s.pAdrMaskList) {
      o["pAdrMaskList"] = boost::python::str(s.pAdrMaskList->c_str());
    } else {
      o["pAdrMaskList"] = ""; 
    }
    return boost::python::incref(o.ptr());
  }

};

template<>
void define_struct_conversion<CFConfig>()
{
  std::cout << "Defining CFConfig" << std::endl;
  Wrap_CFConfig();
}

void define_firmware()
{
  ToPython<DWORD_Four>();
  ToPython<DWORD_Five>();
  ToPython<DWORD_Ten>();
  ToPython<DWORD_Sixteen>();
  //Array_To_List<SimPattern>();
  ToPython<SimPatternFour>();
  ToPython<std_string_p>();

  define_struct<CFDiagnostics>("cf_diagnostics");
  define_struct<CFStatistics>("cf_stats");
  define_struct<CFConfig>("cf_config");
  define_struct<CFSimulation>("cf_simulation");
};
