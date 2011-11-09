#ifndef __ANT_ANIMAL_H
#define __ANT_ANIMAL_H

#include <entity.h>

class AntAnimal:public AntEntity {
public:
    AntAnimal(AntMap *pMap);

    virtual AntAnimal *createOne()=0;
    bool dead();
    void die();
protected:
    bool giveBirth();
private:
    float lastBirth;
    bool mDead;
};

#endif
