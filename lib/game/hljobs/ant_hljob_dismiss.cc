#include "ant_hljob_dismiss.h"
#include "ant_boss.h"

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
  switch(state) {
    case GATHERING: 
      {
         p->setMode(AntPerson::MOVING); 
         p->setMeshState("walk");
         p->newMoveJob(0,getBossEntity()->getPos2D(),0);
      }
      break;
  }
}
