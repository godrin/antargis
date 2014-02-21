#ifndef __ANT_DWELLING_H
#define __ANT_DWELLING_H

#include "ant_house.h"

class AntDwelling:public AntHouse {
public:
    AntDwelling(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antDwelling";
    }

    std::vector<AntStockNeed > neededStock();
    void process();
private:
    int lastBirth;
};

#endif

