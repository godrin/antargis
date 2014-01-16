#ifndef __ANT_FORMATION_SINGLEPOS_H
#define __ANT_FORMATION_SINGLEPOS_H

#include "ant_formation.h"
class AntFormationSinglePos:public AntFormation {
public:
    AntFormationSinglePos(AntBoss *pboss);

protected:
    virtual void sortMen(std::vector<AntPerson*> &tosort);
    virtual std::map<AntPerson*,AGVector2> computeFormation();
};

#endif

