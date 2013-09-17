#ifndef __ANT_HLJOB_DROP_H
#define __ANT_HLJOB_DROP_H

#include "ant_hljob_gather_and_do.h"


class AntHLJobDrop:public AntHLJobGatherAndDo {
  public:
    enum DropType {DROP_WEAPONS, DROP_FOOD };

    AntHLJobDrop(AntBoss *pBoss,DropType pDropType);

  protected:
    virtual void afterGathering();
    void dropSomething();
  private:
    DropType mDropType;
};

#endif

