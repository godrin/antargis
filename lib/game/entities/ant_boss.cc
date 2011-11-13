#include "ant_boss.h"
#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"


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
//    std::cout<<"assign:"<<man<<" "<<typeid(*man).name()<<std::endl;
    if (hlJob) {
        hlJob->assignJob(man);
    } else {
        AntEntity *e=(AntEntity*)(man);
        e->newRestJob(100);
        man->setMeshState("sit");
    }

}
