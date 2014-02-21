#ifndef __ANT_BOAT_H
#define __ANT_BOAT_H

#include "entity.h"

class AntBoat:public AntEntity {
public:
    AntBoat(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antBoat";
    }
};

#endif
