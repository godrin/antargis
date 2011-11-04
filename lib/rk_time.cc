#include "rk_base.h"
#include "rk_time.h"

RKTime::RKTime() {
  time_t *cTime=new time_t;

  mTime=(void*)cTime;

  time(cTime);
}

RKTime::~RKTime()
        {
  time_t *cTime=(time_t*)mTime;
  delete cTime;
  
}


std::string RKTime::toString() {
  time_t *cTime=(time_t*)mTime;
  struct tm * timeinfo;
  char buffer [80];

  timeinfo = localtime ( cTime );

//  strftime (buffer,80,"Now it's %I:%M%p.",timeinfo);
  strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
  return std::string(buffer);
}

