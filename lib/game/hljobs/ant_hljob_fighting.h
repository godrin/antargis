#ifndef __ANT_HLJOB_FIGHTING_H
#define __ANT_HLJOB_FIGHTING_H

#include "ant_hljob_moving.h"

class AntHlJobFighting:public AntHLJobMoving {
  public:
    AntHlJobFighting(AntBoss* pBoss,AntBoss *ptarget);
    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);
  private:
    AntBoss *target;
};

#endif
