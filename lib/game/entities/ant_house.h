#ifndef __ANT_HOUSE_H
#define __ANT_HOUSE_H

#include <ant_boss.h>

#include <set>

class AntMan;

class AntHouse:public AntBoss {
public:
    AntHouse(AntMap* pMap);
    virtual ~AntHouse() throw();
    
    virtual void init();
    virtual void removeMan(AntMan *man);
    
        void setupRing();

private:
  std::set<AntMan*> atHome;
    
};  

#endif
