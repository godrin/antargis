#include "ant_house.h"
#include "ant_hljob_fetching.h"
#include "ant_ring.h"

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
    AntEntity::setupRing();
}


ColoredMesh* AntHouse::getRing()
{
    return makeBigRingMesh(getMap());
}


AntEntity* AntHouse::getEntity()
{
    return this;
}

void AntHouse::eventNoHlJob()
{
    setHlJob(new AntHLJobFetching(this));
}

void AntHouse::loadXML(const Node& node)
{
    AntEntity::loadXML(node);
    AntBoss::loadXMLBoss(node);
}
