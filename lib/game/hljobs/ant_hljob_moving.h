#ifndef __ANT_HLJOB_MOVING_H
#define __ANT_HLJOB_MOVING_H

#include "ant_hljob.h"
#include "ant_man.h"

class AntHLJobMoving:public AntHLJob {
public:
    enum JOB_STATE { FORMAT,MOVING};

    AntHLJobMoving(AntBoss* pBoss);
    AntHLJobMoving(AntBoss* pBoss,const AGVector2 &pTargetPosition,float dist,bool doFormat=true, bool doNothingAtAll=false);
    void initMoving(const AGVector2 &pTargetPosition,float dist,bool doFormat=true);

    void init();

    virtual bool finished();
    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);
    virtual bool fireBurning() const;
protected:
    virtual AntHero *getHero();

    virtual bool checkPerson(AntPerson *p);
    virtual void eventMoveFinished();
    virtual void dontMoveAnymore();
private:

    AGVector2 formatDir();
    std::vector< AntPerson* > getMen();
    void startFormatting();
    void startWalking();

    size_t countMen(AntMan::JobMode mode);

    std::list<AGVector2> waypoints;
    AGVector2 targetPosition;
    AGVector2 currentTarget;

    float dist;
    float formatDist;
    bool shouldFormat;

    bool moveFinished;

    JOB_STATE state;
};

#endif
