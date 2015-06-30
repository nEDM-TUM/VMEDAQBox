#ifndef _Error_hh_
#define _Error_hh_
#include <stdexcept>
#include <string>

namespace cascade {
  class daqbox_error: public std::runtime_error
  {
    public:
      daqbox_error(const std::string& msg) : std::runtime_error(msg) {}
  };
}



#endif /*_Error_hh_*/

