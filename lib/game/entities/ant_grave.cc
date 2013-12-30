#include "ant_grave.h"
#include "ant_models.h"
#include "map.h"

AntGrave::AntGrave(AntMap* pMap, AntGrave::Type ptype): AntEntity(pMap),type(ptype)
{
    waited=false;
}


void AntGrave::init()
{
  AntEntity::init();

  setMesh("grave",type==HERO?"hero":"man");

}


void AntGrave::eventNoJob()
{
  AntEntity::eventNoJob();

  if (waited) {
    getMap()->removeEntity(this);
  } else {
    newRestJob(40);
    waited=true;
  }
}
