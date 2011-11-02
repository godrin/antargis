#include <ag_collector.h>
#include <rk_debug.h>
#include <ag_main.h>

AGCollector::~AGCollector() throw()
  {

  }

void AGCollector::insertGlobal(AGRubyObject *pObject)
  {
    // cdebug(pObject<<":"<<typeid(*pObject).name());
    mGlobals.insert(pObject);
  }

void AGCollector::removeGlobal(AGRubyObject *pObject)
  {
    //cdebug(pObject<<":"<<typeid(*pObject).name());
    mGlobals.erase(pObject);
  }

void AGCollector::mark() throw()
  {
    for(std::set<AGRubyObject*>::iterator i=mGlobals.begin();i!=mGlobals.end();i++)
      {
        // cdebug(*i);

        markObject(*i);
      }
  }

AGCollector *getCollector()
  {
    if(getMain())
      return getMain()->getCollector();
    return 0;
  }
