#ifndef RK_TIME_H
#define RK_TIME_H

#include <string>

class RKTime
{
public:
  RKTime();
  ~RKTime();
  std::string toString();
private:
  void *mTime;
};

#endif
