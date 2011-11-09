#ifndef __ANT_SHEEP_H
#define __ANT_SHEEP_H

#include "ant_animal.h"

class AntSheep:public AntAnimal {
public:
    AntSheep(AntMap* pMap);
    virtual ~AntSheep() throw();
    void init();
    virtual void eventNoJob();
    virtual AntSheep *createOne();
    void setMeshState(const AGString& arg1);

private:
    AGVector2 getTargetPos();

    bool wasdead;
};

#endif
