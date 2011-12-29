#ifndef __ANT_FORMATION_BLOCK_H
#define __ANT_FORMATION_BLOCK_H

#include "ant_formation.h"

class AntFormationBlock:public AntFormation {
public:
    AntFormationBlock(AntBoss* pboss,const AGVector2 &pdir);
protected:
    virtual std::map<AntPerson*,AGVector2> computeFormation();
    virtual void sortMen(std::vector<AntPerson*> &tosort);
private:
    AGVector2 dir;
    std::map<AntPerson*,AGVector2> realPositions;
    bool inited;

};

#endif
