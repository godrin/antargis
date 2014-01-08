#include "ant_hljob_pickup_from.h"
#include "ant_animal.h"
#include "ant_hero.h"
#include "ant_formation_rest.h"
#include <algorithm>

AntHLJobPickupFrom::AntHLJobPickupFrom(AntBoss* pBoss, AntBoss *pTarget,const AGString &what): 
  AntHLJobMoving(pBoss,pTarget->getEntity()->getPos2D(),3) {
    CTRACE;
    mFinished=false;
    mTarget=pTarget;
    state=START;
    mResourceToTake=what;
  }

bool AntHLJobPickupFrom::finished() {
  return mFinished;
}

bool AntHLJobPickupFrom::checkPerson ( AntPerson* person )
{
  if ( AntHLJobMoving::finished() ) {
    switch(state) {
      case NEARLY_FINISHED: 
        {
          takeIt();
          state=GO_BACK;
        }
        break;
      case TAKE:
        {
          person->newRestJob(1);
          state=NEARLY_FINISHED;
        }
        break;
      case GO_BACK:
        if(moveTo(person,storedPos,true)) {
          mFinished=true;
          return true;
        }
        break;
      default:
        {
          getHero()->setFormation ( new AntFormationRest ( getHero(), AntHero::FIRE_DISPLACE) );
          AGVector2 pos=mTarget->getEntity()->getPos2D();
          storedPos=getBossEntity()->getPos2D();
          state=TAKE;

          for(auto menIter:getMenWithBoss()) {
            menIter->newMoveJob(0,pos,0);
          }
        }
        break;
    }
  } else {
    AntHLJobMoving::checkPerson(person);
  }
  return false;
}

void AntHLJobPickupFrom::takeIt() {
  CTRACE;
  getHero()->resource.take(mTarget->getEntity()->resource,mResourceToTake,getBossEntity()->getAggression()/3.0);
}


