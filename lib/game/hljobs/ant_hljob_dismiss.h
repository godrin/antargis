#ifndef __ANT_HLJOB_DISMISS_H
#define __ANT_HLJOB_DISMISS_H

#include "ant_hljob_gather_and_do.h"

class AntHLJobDismiss:public AntHLJobGatherAndDo {
  public:
    AntHLJobDismiss(AntBoss *pBoss);

    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);

  protected:
    virtual void afterGathering();
    void detachSomeMen();
  private:
    int toDismiss;
};

#endif

