#include "ant_boss.h"
#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"
#include "ant_man.h"
#include "ant_formation.h"


AntBoss::AntBoss ( ):hlJob(0) {
}

AntBoss::~AntBoss() throw() {

}

void AntBoss::setupMeshBoss() {
    setupMesh();
    setupRing();
}

void AntBoss::init() {
    setupMeshBoss();
}

void AntBoss::removeMan(AntMan* man)
{
    menWithoutBoss.remove(man);
}

void AntBoss::signUp(AntMan* man)
{
    menWithoutBoss.push_back(man);
}


void AntBoss::assignJob(AntMan* man)
{
    CTRACE;
    if (!hlJob)
        eventNoHlJob();

//    std::cout<<"assign:"<<man<<" "<<typeid(*man).name()<<std::endl;
    if (hlJob) {
        cdebug("HLJOB:"<<typeid(*hlJob).name());
        hlJob->check(man);
    } else {
        AntEntity *e=(AntEntity*)(man);
        e->newRestJob(100);
        man->setMeshState("sit");
    }

}
void AntBoss::assignJob(AntHero* man)
{
    CTRACE;
//    std::cout<<"assign:"<<man<<" "<<typeid(*man).name()<<std::endl;
    if (!hlJob)
        eventNoHlJob();

    if (hlJob) {
        hlJob->check(man);
    } else {
        AntEntity *e=(AntEntity*)(man);
        e->newRestJob(100);
        //man->setMeshState("sit");
    }

}

std::vector< AntPerson* > AntBoss::getMenWithBoss()
{
    std::vector< AntPerson* > l;
    l.push_back(dynamic_cast<AntPerson*>(getEntity()));
    std::copy(menWithoutBoss.begin(),menWithoutBoss.end(),std::back_inserter(l));
    return l;
}
std::vector< AntPerson* > AntBoss::getMenWithoutBoss()
{
    std::vector< AntPerson* > l;
    std::copy(menWithoutBoss.begin(),menWithoutBoss.end(),std::back_inserter(l));
    return l;
}

AntFormation* AntBoss::getFormation()
{
    return formation;
}

AGVector2 AntBoss::getFormation(AntPerson* e, const AGVector2& v)
{
    cdebug("getFormation:"<<formation);
    if (formation) {
        return formation->getPosition(e,v);
    } else
        return getEntity()->getPos2D();
}

void AntBoss::setHlJob(AntHLJob* job)
{
    delete hlJob;
    hlJob=job;
}

void AntBoss::setFormation(AntFormation* pformation)
{
    formation=pformation;
}
