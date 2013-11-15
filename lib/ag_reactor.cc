#include <boost/signals.hpp>
#include <iostream>

int testing() {

  boost::signal<void()> a;

  a.connect([]{
      std::cout<<"HI"<<std::endl;
      });
  a();
  return 0;
}
