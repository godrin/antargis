#ifndef __ANT_HLJOB_PICKUP_FROM_H
#define __ANT_HLJOB_PICKUP_FROM_H

#include "ant_hljob_moving.h"

class AntAnimal;

class AntHLJobPickupFrom: public AntHLJobMoving {
  public:
    AntHLJobPickupFrom(AntBoss* pBoss, AntBoss *pTarget,const AGString &what);

    bool finished();
    void checkPerson ( AntPerson* person );

  private:
    void takeIt();

    enum PickUpState { START, TAKE, GO_BACK, NEARLY_FINISHED };
    PickUpState state;
    bool mFinished;
    AntBoss *mTarget;
    AGString mResourceToTake;
    AGVector2 storedPos;
};

#endif

