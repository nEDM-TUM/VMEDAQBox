#include <iostream>
#include "WrapDAQBox.hh"
#include "Configuration.hh"
#include "autobahn/autobahn.hpp"
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <boost/any.hpp>
using namespace std;
using namespace boost;
using namespace autobahn;

using boost::asio::ip::tcp;


int main()
{
   try {
      // ASIO service object
      //
      asio::io_service io;

      // the TCP socket we connect
      //
//      tcp::socket socket(io);
      tcp::socket socket(io, tcp::v4());

      // setting this option minimizes latency at some cost
      //
      socket.set_option(tcp::no_delay(true));

      // connect to this server/port
      //
      tcp::resolver resolver(io);
      auto endpoint_iterator = resolver.resolve({"127.0.0.1", "8090"});

      // create a WAMP session that talks over TCP
      //
      bool debug = false;
      cascade::session_type session(io, socket, socket, debug);

      // make sure the future returned from the session joining a realm (see below)
      // does not run out of scope (being destructed prematurely ..)
      //
      future<void> session_future;

      // same for other vars we need to keep alive ..
      //

      // now do an asynchronous connect ..
      //
      boost::asio::async_connect(socket, endpoint_iterator,

         // we either connected or an error happened during connect ..
         //
         [&](boost::system::error_code ec, tcp::resolver::iterator) {

            if (!ec) {
               cerr << "Connected to server" << endl;

               // start the WAMP session on the transport that has been connected
               //
               session.start();

               // join a realm with the WAMP session
               //
               session_future = session.join("realm1").then([&](future<uint64_t> s) {

                  cerr << "Session joined to realm with session ID " << s.get() << endl;

                  // REGISTER a procedure for remote calling
                  //
		  cascade::define_daqbox_interface(session);

               });

            } else {
               cerr << "Could not connect to server: " << ec.message() << endl;
            }
         }
      );

      cerr << "Starting ASIO I/O loop .." << endl;

      io.run();

      cerr << "ASIO I/O loop ended" << endl;
   }
   catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }
   return 0;
}
