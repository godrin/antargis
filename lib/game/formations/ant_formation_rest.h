#ifndef __ANT_FORMATION_REST_H
#define __ANT_FORMATION_REST_H

#include "ant_formation.h"

class AntFormationRest:public AntFormation {
public:
    AntFormationRest(AntBoss* pboss,const AGVector2 &displace);

    std::map<AntPerson*,AGVector2> computeFormation();


protected:
    AGVector2 getPositionReal(AntPerson *p);
    void sortMen(std::vector<AntPerson*> &v);
private:
    AGVector2 displacement;
};

#endif
