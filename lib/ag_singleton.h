#ifndef AGSINGLETON
#define AGSINGLETON

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_rubyobj.h>

class AGEXPORT AGSingleton:public AGRubyObject
{
 public:
  AGSingleton();
  virtual ~AGSingleton() throw ();
};

#endif
