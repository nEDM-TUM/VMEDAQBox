#ifndef _Configuration_hh_
#define _Configuration_hh_
#include "autobahn/autobahn.hpp"
#include <boost/asio.hpp>

namespace cascade {
  typedef boost::asio::ip::tcp tcp;
  typedef autobahn::session<tcp::socket,
                        tcp::socket> session_type;
}
#endif
