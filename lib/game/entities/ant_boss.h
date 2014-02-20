#ifndef __ANT_BOSS_H
#define __ANT_BOSS_H

#include "entity.h"
#include "ant_person.h"

class AntMan;
class AntHero;
class AntPerson;
class AntHLJob;
class AntFormation;
class AntPlayer;

struct AntStockNeed {
  std::string resource;
  float amount,currentNeed;
  AntStockNeed(const std::string &r,float amount,float cNeed);

  bool operator<(const AntStockNeed &o) const;
};


class AntBoss
{
public:
  AntBoss();
  virtual ~AntBoss() throw ();

  void setupMeshBoss();
  virtual void setupMesh() =0;
  virtual void setupRing() =0;

  virtual void init();

  void signUp ( AntMan*man );
  void removeMan ( AntMan*man );

  virtual void assignJob ( AntPerson *person );

  virtual AntEntity *getEntity() =0;

  AntFormation *getFormation();
  AGVector2 getFormation ( AntPerson*e,const AGVector2 &v );
  std::vector<AntPerson*> getMenWithoutBoss();
  std::vector<AntPerson*> getMenWithBoss();
  std::vector<AntPerson*> getMenWithoutBoss(AntPerson::JobMode mode);

  virtual void eventNoHlJob() =0;

  virtual void setHlJob ( AntHLJob *job );
  AntHLJob *getHlJob();
  void setFormation ( AntFormation *formation );

  void setPlayer ( AntPlayer *player );
  AntPlayer *getPlayer();
  virtual AntMap *getMap() =0;
  virtual int getID() =0;
  std::vector<AntPerson*> getRestingMenWithHero();

  size_t menCount() const;
  void delJobs();

  virtual std::vector< AntStockNeed > neededStock();
protected:
  void checkHlJobEnd();

  void eventLostMan(AntPerson *person);

  void saveXMLBoss ( Node &node ) const;
  void loadXMLBoss ( const Node &node );

private:
  std::list<AntMan*> menWithoutBoss;
  AntHLJob *hlJob;
  AntFormation *formation;
  AntPlayer *player;

  size_t menToAddCount;
};

#endif
