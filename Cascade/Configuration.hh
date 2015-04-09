#ifndef _Configuration_hh
#define _Configuration_hh
#include "autobahn/autobahn.hpp"
#include <boost/asio.hpp>

namespace cascade {
  typedef boost::asio::ip::tcp tcp;
  typedef autobahn::session<tcp::socket,
                        tcp::socket> session_type;
}
#endif
