#include <ant_mine.h>
#include <ant_models.h>

AntMine::AntMine(AntMap* pMap): AntHouse(pMap)
{

}
void AntMine::init()
{
  AntHouse::init();
  setProvide("coal",true);
  setProvide("ore",true);

  resource.set("coal",500);
  resource.set("ore",500);
  setDirection(40);
}

void AntMine::setupMesh()
{
  setMesh("mine");
}

