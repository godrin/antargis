#ifndef __ANT_ANIMAL_H
#define __ANT_ANIMAL_H

#include <entity.h>

class AntAnimal:public AntEntity {
public:
    AntAnimal(AntMap *pMap);

    void init();

    virtual AntAnimal *createOne()=0;
    bool dead();
    virtual void die();

    void eventNoJob();
protected:
    bool giveBirth();
    virtual void moveAround();
private:
    float mLastBirth;
    bool mDead;
    bool mAlreadyDead;
    size_t mFood;
};

#endif
