#include "ant_hljob_dismiss.h"
#include "ant_boss.h"
#include "ant_man.h"

AntHLJobDismiss::AntHLJobDismiss(AntBoss *pBoss):AntHLJob(pBoss){
  AntEntity *e=dynamic_cast<AntEntity*>(pBoss);
  toDismiss = pBoss->getMenWithoutBoss().size()*pBoss->getEntity()->getAggression()/3;
  state = GATHERING;
  assignAll();
}


bool AntHLJobDismiss::finished() {
  return state == FINISHED;
}

void AntHLJobDismiss::checkPerson(AntPerson *p) {
  AGVector2 centerPos=getBossEntity()->getPos2D();
  switch(state) {
    case GATHERING: 
      {
        if((centerPos-p->getPos2D()).length()<0.2) {
          p->setMode(AntPerson::READY);
          p->setMeshState("stand");
          p->newRestJob(5);
        } else {
          p->setMode(AntPerson::MOVING); 
          p->setMeshState("walk");
          p->newMoveJob(0,centerPos,0);
        }
        if(getBoss()->getMenWithoutBoss(AntPerson::READY).size()==getBoss()->menCount()) {
          detachSomeMen();
        }
      }
      break;
  }
}


void AntHLJobDismiss::detachSomeMen() {
  auto allMen=getBoss()->getMenWithoutBoss();
  size_t toDismiss=allMen.size()*getBossEntity()->getAggression()/3;
  for(auto person:allMen) {
    if(toDismiss==0)
      break;
    AntMan *man=dynamic_cast<AntMan*>(person);
    if(man) {
      getBoss()->removeMan(man);
      man->setBoss(0);
      man->delJob();
    }
    toDismiss--;
  }

  state = FINISHED;

  for(auto person:getBoss()->getMenWithBoss()) {
    person->delJob();
  }

  std::cout<<"SET STATE TO "<<state<<" "<<FINISHED<<std::endl;
}
