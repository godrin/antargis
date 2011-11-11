#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"

AntHLJob::AntHLJob(AntBoss* pBoss):mBoss(pBoss)
{

}

void AntHLJob::assignJob(AntPerson* p)
{
    AntEntity *e=reinterpret_cast<AntEntity*>(p);
    e->newRestJob(10);
}
