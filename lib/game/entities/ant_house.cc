#include "ant_house.h"
#include "ant_hljob_fetching.h"

AntHouse::AntHouse(AntMap* pMap): AntEntity(pMap)
{

}

AntHouse::~AntHouse() throw()
{

}

void AntHouse::init()
{
    AntEntity::init();
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

AntEntity* AntHouse::getEntity()
{
    return this;
}

void AntHouse::eventNoHlJob()
{
    setHlJob(new AntHLJobFetching(this));
}
