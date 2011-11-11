#include "ant_house.h"

AntHouse::AntHouse(AntMap* pMap): AntBoss(pMap)
{

}

AntHouse::~AntHouse() throw()
{

}

void AntHouse::init()
{
    AntBoss::init();
    setProvide("house",true);
}

void AntHouse::removeMan(AntMan* man)
{
    atHome.erase(man);
}

void AntHouse::setupRing()
{
    cdebug("NOT IMPLEMENTED");
}
