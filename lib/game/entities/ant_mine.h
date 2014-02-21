#ifndef __ANT_MINE_H
#define __ANT_MINE_H

#include "ant_house.h"

class AntMine:public AntHouse {
public:
    AntMine(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antMine";
    }

};

#endif
