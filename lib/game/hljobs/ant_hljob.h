#ifndef __HL_JOB_H
#define __HL_JOB_H

#include <ag_geometry.h>

#include <vector>

class AntBoss;
class AntMan;
class AntHero;
class AntEntity;
class AntPerson;

class AntHLJob {
public:
  AntHLJob(AntBoss *pBoss);
  
  virtual void check(AntMan *p)=0;
  virtual void check(AntHero *p)=0;
  
  AntBoss *getBoss();
  AntEntity *getBossEntity();
  std::vector<AntPerson*> getMenWithoutBoss();
  std::vector<AntPerson*> getMenWithBoss();
  
protected:
  virtual AGVector2 basePos()=0;
  
private:
  AntBoss *mBoss;
};

#endif
