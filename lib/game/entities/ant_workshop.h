#ifndef __ANT_WORKSHOP_H
#define __ANT_WORKSHOP_H

#include "ant_house.h"

class AntWorkshop:public AntHouse {
public:
    AntWorkshop(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antWorkshop";
    }
};

#endif
