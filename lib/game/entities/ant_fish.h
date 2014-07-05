#ifndef __ANT_FISH_H
#define __ANT_FISH_H

#include "ant_animal.h"

class AntFish:public AntAnimal {
public:
    AntFish(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antFish";
    }
    virtual AntFish *createOne();
protected:
    void moveAround();
    AGVector2 getTargetPos();
};

#endif

