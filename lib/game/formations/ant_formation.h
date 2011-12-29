#ifndef __ANT_FORMATION_H
#define __ANT_FORMATION_H

#include <ag_geometry.h>

#include <list>
#include <map>

class AntPerson;
class AntBoss;

class AntFormation {
public:
    AntFormation(AntBoss *pboss);

    AGVector2 getPosition(AntPerson *e,const AGVector2 &v);

protected:
    bool changedMen();
    virtual void sortMen(std::vector<AntPerson*> &tosort)=0;
    virtual std::map<AntPerson*,AGVector2> computeFormation()=0;
    std::vector<AntPerson*> getSortedMen();

    AntBoss *getBoss();

private:
    AGVector2 getPositionReal(AntPerson *man);


    AntBoss *boss;
    std::vector<AntPerson*> persons;
    std::map<AntPerson*,AGVector2> cache;

    std::map<AntPerson*,AGVector2> realPositions;
    bool inited;


};

class AntManSortBetterWeapon {
public:
    bool operator()(AntPerson *a,AntPerson *b) const;
};

#endif
