#include <ant_farm.h>
#include <ant_models.h>

AntFarm::AntFarm(AntMap* pMap): AntHouse(pMap)
{

}
void AntFarm::init()
{
    AntHouse::init();
    setProvide("farm",true);
}

void AntFarm::setupMesh()
{
  setMesh("farm");
}

