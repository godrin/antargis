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
    void eventNoJob();
private:
    AGVector2 getTargetPos();
    int mFoodAdd;
    bool alreadyDead;
};

#endif

