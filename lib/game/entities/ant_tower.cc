#include <ant_tower.h>
#include <ant_models.h>

AntTower::AntTower(AntMap* pMap): AntHouse(pMap)
{

}
void AntTower::init()
{
    AntHouse::init();
    setProvide("tower",true);
}

void AntTower::setupMesh()
{
    setMesh(AntModels::createModel(getScene(),"tower",""));

}
