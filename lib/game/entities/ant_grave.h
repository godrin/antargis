#ifndef __ANT_GRAVE_H
#define __ANT_GRAVE_H

#include "entity.h"


class AntGrave:public AntEntity {
public:
    enum Type {NORMAL,HERO};
    AntGrave(AntMap* pMap,Type ptype);
    void init();
    
    void eventNoJob();

    virtual AGString xmlName() const {
      return "antGrave";
    }
private:
    Type type;
    bool waited;
};

#endif
