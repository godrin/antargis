#ifndef __ANT_ENTITY_SELECTOR_H
#define __ANT_ENTITY_SELECTOR_H

#include <map.h>

template<class T>
class AntEntityClassSelector:public AntEntitySelector {
public:
  bool ok(AntEntity *e) {
    return dynamic_cast<T*>(e);
  }
};

#endif
