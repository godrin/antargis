#ifndef __ANT_TOWER_H
#define __ANT_TOWER_H

#include "ant_house.h"

class AntTower:public AntHouse {
public:
    AntTower(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antTower";
    }
};

#endif
