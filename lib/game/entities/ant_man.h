#ifndef __ANT_MAN_H
#define __ANT_MAN_H

#include "entity.h"
#include "ant_person.h"

class AntMan:public AntEntity,public AntPerson {
public:
    AntMan(AntMap* pMap);
};

#endif
