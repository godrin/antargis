#include "ant_hljob_gather_and_do.h"
#include "ant_boss.h"
#include "ant_man.h"

AntHLJobGatherAndDo::AntHLJobGatherAndDo(AntBoss *pBoss):AntHLJob(pBoss){
  state = INIT;
}


bool AntHLJobGatherAndDo::finished() {
  return state == FINISHED;
}

void AntHLJobGatherAndDo::checkPerson(AntPerson *p) {
  AGVector2 centerPos=getBossEntity()->getPos2D();
  switch(state) {
    case INIT:
      state=GATHERING;
      assignAll();
      break;
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
          state=FINISHED;
          p->setMeshState("walk");
          afterGathering();
        }
      }
      break;
  }
}

