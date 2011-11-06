#ifndef __ANT_BOSS_H
#define __ANT_BOSS_H

#include "entity.h"

class AntPerson;

class AntBoss:public AntEntity {
public:
    AntBoss(AntMap* pMap);
    virtual ~AntBoss() throw ();
    
    void setupMeshBoss();
    virtual void setupMesh()=0;
    virtual void setupRing()=0;
    
private:
  std::vector<AntPerson*> men;
};

#endif
