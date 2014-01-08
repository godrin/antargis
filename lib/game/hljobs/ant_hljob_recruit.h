#ifndef __ANT_HLJOB_RECRUIT_H
#define __ANT_HLJOB_RECRUIT_H

#include "ant_hljob_moving.h"

class AntHlJobRecruit:public AntHLJobMoving
{
  public:
    AntHlJobRecruit ( AntBoss* pBoss,AntBoss *pTarget );

    bool finished();

  protected:
    AGVector2 basePos();
    void eventMoveFinished();
    bool checkPerson ( AntPerson *person );
  private:
    void doCollect();
    AntBoss *getTargetBoss();
    void setBasePos();


    void reorderSittingMen();

    int targetId;
    bool fetchingStarted;
    bool fetchingFinished;

    std::map<int,int> manMap;
    AGVector2 mBasePos;
    int wantedMen;
};


#endif
