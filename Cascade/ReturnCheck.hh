#ifndef _ReturnCheck_hh_
#define _ReturnCheck_hh_

#include <map>
#include <boost/any.hpp>

namespace cascade {

typedef std::map<std::string, boost::any> check_return_map;
extern check_return_map gReturnMap;

template<typename T>
void RegisterTypeCheck(std::function<T(const T&)> func)
{
  gReturnMap[typeid(T).name()] = func;
}

template<typename T>
T PerformCheck(const T& val)
{
  auto check = gReturnMap.find(typeid(T).name());
  if (check == gReturnMap.end()) return val;
  return boost::any_cast<std::function<T(const T&)> >(check->second)(val);
}


}

#endif /*_ReturnCheck_hh_*/
