#ifndef __ANT_HLJOB_FETCHING_H
#define __ANT_HLJOB_FETCHING_H

#include "ant_hljob.h"

class AntHLJobFetching:public AntHLJob {
public:
    AntHLJobFetching(AntBoss* pBoss);

    virtual void check(AntMan*);
    virtual void check(AntHero*);
    virtual AGVector2 basePos();

};

#endif
