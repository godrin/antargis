#include <ant_boat.h>
#include <ant_models.h>
#include <map.h>
#include <ant_field.h>
#include <ag_main.h>

AntBoat::AntBoat(AntMap* pMap): AntEntity(pMap)
{

}
void AntBoat::init()
{
  AntEntity::init();
  setProvide("boat",true);
}

void AntBoat::setupMesh()
{
  setMesh("boat");
}

