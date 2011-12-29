#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"
#include "ant_boss.h"

AntHLJob::AntHLJob(AntBoss* pBoss):mBoss(pBoss)
{

}

AntBoss* AntHLJob::getBoss()
{
    return mBoss;
}

std::vector< AntPerson* > AntHLJob::getMenWithBoss()
{
    return mBoss->getMenWithBoss();
}

std::vector< AntPerson* > AntHLJob::getMenWithoutBoss()
{
    return mBoss->getMenWithoutBoss();
}



AntEntity* AntHLJob::getBossEntity()
{
    return mBoss->getEntity();
}

AntMap* AntHLJob::getMap()
{
    return getBossEntity()->getMap();
}
