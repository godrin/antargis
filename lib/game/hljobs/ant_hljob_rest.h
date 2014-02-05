#ifndef __ANT_HLJOB_REST_H
#define __ANT_HLJOB_REST_H

#include "ant_hljob.h"
#include "rk_string.h"

class AntEntity;
class AntPerson;

class AntHLJobRest:public AntHLJob {
public:
    AntHLJobRest(AntBoss* pBoss);
    AntHLJobRest(AntBoss* pBoss,float pTime);

    virtual void init();

    bool checkPerson(AntPerson *man);
    bool finished();
    
    AGVector2 basePos();
  
    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);
private:
    void initRestJob();

    bool heroHasFood();
    void eat(AntPerson* man);
    void spreadThings();
    void spreadThing(const AGString &what);
    void spreadFood();

    float mTime;
    bool firstTime;
    bool spreadingThings;
    bool jobFinished;
};

#endif
