#ifndef __ANT_HLJOB_FIGHTING_H
#define __ANT_HLJOB_FIGHTING_H

#include "ant_hljob_moving.h"
#include <set>

class AntHlJobFighting:public AntHLJobMoving {
  public:
    AntHlJobFighting(AntBoss* pBoss,AntBoss *ptarget, bool attacking=true);
    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);

    void removeFightingperson(AntPerson *person);
  protected:
    bool checkPerson ( AntPerson* person );
    bool fight(AntPerson *person);
    bool finished();
  private:
    void startFighting();
    void reactOnWon();

    enum State {START, FIGHTING, WON, FINISHED};
    State state;
    AntBoss *target;
    bool mAttacking;
    AGVector2 startPos;
    std::set<int> fightingMenIDs;
};

#endif
