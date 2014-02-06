#include <ant_workshop.h>
#include <ant_models.h>

AntWorkshop::AntWorkshop(AntMap* pMap): AntHouse(pMap)
{

}
void AntWorkshop::init()
{
    AntHouse::init();
    setProvide("workshop",true);
}

void AntWorkshop::setupMesh()
{
  setMesh("workshop");
}
