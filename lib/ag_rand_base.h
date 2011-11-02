#ifndef AG_RAND_BASE_H
#define AG_RAND_BASE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_rubyobj.h>

class AGEXPORT AGRandomizerBase:public AGRubyObject
{
 public:
#ifdef SWIG
  %rename(randFloat) operator()(float f);
  %rename(randInt) operator()(int i);
#endif
  virtual float operator()(float f);
  virtual int operator()(int i);

};

#endif

