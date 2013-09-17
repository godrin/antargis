#ifndef __HL_JOB_H
#define __HL_JOB_H

#include <ag_geometry.h>

#include <vector>

class AntBoss;
class AntMan;
class AntHero;
class AntEntity;
class AntPerson;
class AntMap;

class AntHLJob
{
public:
  AntHLJob ( AntBoss *pBoss );

  virtual void checkPerson ( AntPerson *p );
  virtual bool finished() =0;

  AntBoss *getBoss();
  AntEntity *getBossEntity();
  std::vector<AntPerson*> getMenWithoutBoss();
  std::vector<AntPerson*> getMenWithBoss();
  void assignAll();

protected:
  virtual AntMap *getMap();
  void sit ( AntPerson* man );
  virtual AGVector2 basePos();

private:
  AntMap *mMap;
  int heroId;
};

#endif
