#ifndef __ANT_FORMATION_REST_H
#define __ANT_FORMATION_REST_H

#include "ant_formation.h"

class AntFormationRest:public AntFormation {
public:
    AntFormationRest(AntBoss* pboss);

    void computeFormation();


protected:
    AGVector2 getPositionReal(AntPerson *p);
    void sortMen(std::vector<AntPerson*> &v);
private:
    std::map<AntPerson*,AGVector2> realPositions;
    bool inited;
};

#endif
