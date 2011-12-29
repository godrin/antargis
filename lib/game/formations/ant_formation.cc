#include "ant_formation.h"
#include "ant_boss.h"
#include "ant_person.h"

AntFormation::AntFormation(AntBoss* pboss):boss(pboss)
{
    inited=false;
}

AGVector2 AntFormation::getPosition(AntPerson* man, const AGVector2& pos)
{
    if (changedMen()) {
        cdebug("CACHE REFRESH");
        cache.clear();
    } else {
        cdebug("CACHE OK");
    }
    std::map<AntPerson*,AGVector2>::iterator i=cache.find(man);
    if (i!=cache.end()) {
        return i->second+pos;
    }
    AGVector2 r=getPositionReal(man);
    cache[man]=r;
    AGVector2 ret= r+pos;
    return ret;
}

bool AntFormation::changedMen()
{
    std::vector<AntPerson*> pnmen=boss->getMenWithoutBoss();
    sortMen(pnmen);
    return !(pnmen==persons);

}

std::vector< AntPerson* > AntFormation::getSortedMen()
{
    if (persons.empty())
    {
        persons=boss->getMenWithoutBoss();
        sortMen(persons);
    }
    return persons;
}


AntBoss* AntFormation::getBoss()
{
    return boss;
}

bool AntManSortBetterWeapon::operator()(AntPerson* a, AntPerson* b) const
{
    return (a->resource.get("bow")<b->resource.get("bow") || (
                a->resource.get("bow")==b->resource.get("bow") &&
                a->resource.get("sword")<b->resource.get("sword")));
}

AGVector2 AntFormation::getPositionReal(AntPerson* p)
{
    if (realPositions.find(p)==realPositions.end())
        realPositions=computeFormation();
    return realPositions[p];
}
