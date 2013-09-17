#include "ant_hljob_dismiss.h"
#include "ant_boss.h"
#include "ant_man.h"

AntHLJobDismiss::AntHLJobDismiss(AntBoss *pBoss):AntHLJobGatherAndDo(pBoss){
  AntEntity *e=dynamic_cast<AntEntity*>(pBoss);
  toDismiss = pBoss->getMenWithoutBoss().size()*pBoss->getEntity()->getAggression()/3;
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

  for(auto person:getBoss()->getMenWithBoss()) {
    person->delJob();
  }
}

void AntHLJobDismiss::afterGathering() {
  detachSomeMen();
}
