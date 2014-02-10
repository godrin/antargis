#ifndef __ANT_FARM_H
#define __ANT_FARM_H

#include "ant_house.h"

class AntFarm:public AntHouse {
public:
    AntFarm(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antFarm";
    }
};

#endif
