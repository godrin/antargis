#ifndef __ANT_SHEEP_H
#define __ANT_SHEEP_H

#include "ant_animal.h"

class AntSheep:public AntAnimal {
public:
    AntSheep(AntMap* pMap);
    virtual ~AntSheep() throw();
    void init();
    virtual AntSheep *createOne();
    void setMeshState(const AGString& arg1);
    virtual void die();

    virtual AGString xmlName() const {
      return "antSheep";
    }
protected:
    void moveAround();

    AGVector2 getTargetPos();
};

#endif
