#include <boost/signals.hpp>

int testing() {

  boost::signal<void()> a;

  a.connect([]{
      std::cout<<"HI"<<std::endl;
      });
  a();
}
