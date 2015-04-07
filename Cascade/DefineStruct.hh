#ifndef DefineStruct_hh
#define DefineStruct_hh
#include "StructConversion.hh"
#include "PrintStruct.hh"
#include <boost/python.hpp>

template <typename Struct>
void define_struct_conversion()
{
  FromPython<Struct>();
  ToPython<Struct>();
}

template<typename Struct>
Struct GetStruct()
{
  return Struct();
}

template<typename Struct>
void define_struct(const std::string& aname)
{
  define_struct_conversion<Struct>();
  boost::python::def(("get_struct_"+aname).c_str(), &GetStruct<Struct>);
  boost::python::def(("print_struct_"+aname).c_str(), &PrintAll<Struct>::print);
}

#endif
