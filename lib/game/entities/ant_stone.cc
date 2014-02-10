#include <ant_stone.h>
#include <ant_models.h>

AntStone::AntStone(AntMap* pMap): AntHouse(pMap)
{
  resource.set("stone",500);
}
void AntStone::init()
{
  AntHouse::init();
  setProvide("stone",true);
}

void AntStone::setupMesh()
{
  setMesh("stone");
}
