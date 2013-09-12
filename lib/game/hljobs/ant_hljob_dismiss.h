#ifndef __ANT_HLJOB_DISMISS_H
#define __ANT_HLJOB_DISMISS_H

#include "ant_hljob.h"

class AntHLJobDismiss:public AntHLJob {
  public:
    AntHLJobDismiss(AntBoss *pBoss);

    virtual bool finished();

  protected:
    virtual void checkPerson(AntPerson *p);

  private:
    enum JOB_STATE {GATHERING,DISMISSING,FINISHED};
    int toDismiss;
    JOB_STATE state;
};

#endif

