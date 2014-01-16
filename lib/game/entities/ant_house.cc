#include "ant_house.h"
#include "ant_hljob_fetching.h"
#include "ant_ring.h"

AntHouse::AntHouse ( AntMap* pMap ) : AntEntity ( pMap )
{

}

AntHouse::~AntHouse() throw()
{

}

void AntHouse::init()
{
  AntEntity::init();
  AntBoss::init();
  setProvide ( "house",true );
}

void AntHouse::removeMan ( AntMan* man )
{
  atHome.erase ( man );
}

void AntHouse::setupRing()
{
  AntEntity::setupRing();
}


ColoredMesh* AntHouse::getRing()
{
  return makeBigRingMesh ( getMap() );
}


AntEntity* AntHouse::getEntity()
{
  return this;
}

void AntHouse::eventNoHlJob()
{
  setHlJob (0);
}
void AntHouse::saveXML(Node & node) const {
  AntEntity::saveXML ( node );
  AntBoss::saveXMLBoss(node);
}
void AntHouse::loadXML ( const Node& node )
{
  AntEntity::loadXML ( node );
  AntBoss::loadXMLBoss ( node );
}
AntMap* AntHouse::getMap()
{
  return AntEntity::getMap();
}

int AntHouse::getID()
{
  return AntEntity::getID();
}

void AntHouse::setHlJob ( AntHLJob* job ) {
  CTRACE;
  if(!job)
    job = new AntHLJobFetching ( this );
  AntBoss::setHlJob(job);

}

