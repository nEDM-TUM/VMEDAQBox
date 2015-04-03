#include <boost/python.hpp>
using namespace boost::python;

void define_firmware(void);
void define_board(void);
void define_daqbox(void);

BOOST_PYTHON_MODULE(pycdtware)
{
  define_firmware();
  define_board();
  define_daqbox();
}
