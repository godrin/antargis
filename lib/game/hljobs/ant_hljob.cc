#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"

AntHLJob::AntHLJob(AntBoss* pBoss):mBoss(pBoss)
{

}

void AntHLJob::assignJob(AntPerson* p)
{
    AntEntity *e=(AntEntity*)(p);
    e->newRestJob(10);
    p->setMeshState("sit");
}
