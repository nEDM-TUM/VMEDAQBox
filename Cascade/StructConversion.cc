#include "StructConversion.hh"
#include <iostream>

cascade::detail::from_convert_map cascade::detail::gFromWebConversionMap;
cascade::detail::convert_map cascade::detail::gToWebConversionMap;

namespace cascade {
namespace detail {

boost::any ConvertFromWeb(const boost::any& a, const std::type_info& id)
{
  auto pr = std::make_pair<std::string, std::string>(a.type().name(), id.name());
  if (gFromWebConversionMap.find(pr) == gFromWebConversionMap.end()) {
    return a;
  }
  return gFromWebConversionMap[pr](a);
}

boost::any ConvertToWeb(const boost::any& a)
{
  if (gToWebConversionMap.find(a.type().name()) == gToWebConversionMap.end()) {
    return a;
  }
  return gToWebConversionMap[a.type().name()](a);
}

class RegistrationBuilder
{
  public:
  RegistrationBuilder()
  {
    RegisterToWebConversion<unsigned int, uint64_t>::Execute();
    RegisterToWebConversion<unsigned long long, uint64_t>::Execute();
    RegisterFromWebConversion<unsigned long, unsigned int>::Execute();
    RegisterFromWebConversion<unsigned long, unsigned long long>::Execute();
  }
};
}
}

static cascade::detail::RegistrationBuilder _gBuilder;

