#ifndef AG_COLLECTOR_H
#define AG_COLLECTOR_H

#include <rk_base.h>
#include <rk_rubyobj.h>

#include <set>

class AGEXPORT AGCollector:public AGRubyObject
{
 public:
   ~AGCollector() throw();
  void insertGlobal(AGRubyObject *pObject);
  void removeGlobal(AGRubyObject *pObject);

  virtual void mark() throw ();
 private:
  std::set<AGRubyObject*> mGlobals;

};

AGEXPORT AGCollector *getCollector();


#endif
