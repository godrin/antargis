#ifndef __ANT_STONE_H
#define __ANT_STONE_H

#include "ant_house.h"

class AntStone:public AntHouse {
public:
    AntStone(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antStone";
    }
};

#endif
