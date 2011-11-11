#include "ant_boss.h"
#include "ant_hljob.h"
#include "entity.h"


AntBoss::AntBoss ( AntMap* pMap ) : AntEntity ( pMap ) {
}

AntBoss::~AntBoss() throw() {

}

void AntBoss::setupMeshBoss() {
    setupMesh();
    setupRing();
}

void AntBoss::init() {
    AntEntity::init();
    setupMeshBoss();
}

void AntBoss::removeMan(AntPerson* man)
{
    men.remove(man);
}

void AntBoss::signUp(AntPerson* man)
{
    men.push_back(man);
}

void AntBoss::assignJob(AntPerson* man)
{
    if (hlJob) {
        hlJob->assignJob(man);
    } else {
        AntEntity *e=reinterpret_cast<AntEntity*>(man);
        e->newRestJob(10);
    }

}
