#ifndef __ANT_BOSS_H
#define __ANT_BOSS_H

#include "entity.h"

class AntPerson;
class AntHLJob;

class AntBoss:public AntEntity {
public:
    AntBoss(AntMap* pMap);
    virtual ~AntBoss() throw ();
    
    void setupMeshBoss();
    virtual void setupMesh()=0;
    virtual void setupRing()=0;

    virtual void init();
    
    void signUp(AntPerson*man);
    void removeMan(AntPerson*man);
    
    void assignJob(AntPerson *man);
    
private:
  std::list<AntPerson*> men;
  AntHLJob *hlJob;
};

#endif
