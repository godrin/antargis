#ifndef __ANT_HLJOB_GATHER_AND_DO_H
#define __ANT_HLJOB_GATHER_AND_DO_H

#include "ant_hljob.h"

class AntHLJobGatherAndDo:public AntHLJob {
  public:
    AntHLJobGatherAndDo(AntBoss *pBoss);

    virtual bool finished();

  protected:
    virtual void checkPerson(AntPerson *p);
    virtual void afterGathering()=0;

  private:

    enum JOB_STATE {INIT,GATHERING,DISMISSING,FINISHED};
    JOB_STATE state;
};

#endif

