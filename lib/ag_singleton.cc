#include "ag_singleton.h"
#include "ag_main.h"
#include "rk_debug.h"


AGSingleton::AGSingleton()
{
  //  CTRACE;
  assert(hasMain());
  assert(getMain()->getCollector());

  getMain()->getCollector()->insertGlobal(this);
}
AGSingleton::~AGSingleton() throw()
{
  if(!hasMain())
    return;
  assert(hasMain());
  assert(getMain()->getCollector());

  getMain()->getCollector()->removeGlobal(this);
}
