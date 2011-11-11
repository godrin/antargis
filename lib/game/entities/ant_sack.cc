#include "ant_sack.h"
#include "ant_models.h"
#include "map.h"


AntSack::AntSack(AntMap* pMap): AntEntity(pMap)
{

}


void AntSack::init()
{
    AntEntity::init();

    setMesh(AntModels::createModel(getScene(),"sack"));

}


void AntSack::eventDie()
{
    AntEntity::eventDie();
    getMap()->removeEntity(this);
}

void AntSack::resourceChanged()
{
    AntEntity::resourceChanged();

    checkResource("food");
    checkResource("tool");
    checkResource("corn");
    checkResource("stone");
    checkResource("wood");
}


void AntSack::checkResource(const AGString& s)
{
    setProvide(s,resource.get(s)>0);
}
