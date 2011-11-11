#ifndef __ANT_GRAVE_H
#define __ANT_GRAVE_H

#include "entity.h"


class AntGrave:public AntEntity {
public:
    enum Type {NORMAL,HERO};
    AntGrave(AntMap* pMap,Type ptype);
    void init();
    
    void eventNoJob();
private:
    Type type;
    bool waited;
};

#endif
