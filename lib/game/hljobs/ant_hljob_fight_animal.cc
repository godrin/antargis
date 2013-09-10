#include "ant_hljob_fight_animal.h"
#include "ant_animal.h"
#include "ant_hero.h"
#include <algorithm>

AntHLJobFightAnimal::AntHLJobFightAnimal(AntBoss* pBoss, AntAnimal *pTarget): 
    AntHLJobMoving(pBoss,pTarget->getPos2D(),3) {
  CTRACE;
  mFinished=false;
  mTarget=pTarget;
  toEat=0;
  state=START;
}

bool AntHLJobFightAnimal::finished() {
  return mFinished;
}

void AntHLJobFightAnimal::checkPerson ( AntPerson* person )
{
  if ( AntHLJobMoving::finished() ) {
    switch(state) {
      case STATE_EAT:
        {

          person->newRestJob(1);
          if(toEat--<0) {
            killAnimal();
            mFinished=true;
          }
        }
        break;
      default:
        {
          AGVector2 pos=mTarget->getPos2D();
          state=STATE_EAT;

          for(auto menIter:getMenWithBoss()) {
            menIter->newMoveJob(0,pos,0);
          }
        }
        break;
    }
  } else {
    AntHLJobMoving::checkPerson(person);
  }
}

void AntHLJobFightAnimal::killAnimal() {
  CTRACE;
  mTarget->die(); //eventDie();
  getHero()->resource.takeAll(mTarget->resource);
}
