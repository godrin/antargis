#ifndef __ANT_PLAYER_H
#define __ANT_PLAYER_H

#include <rk_string.h>

#include <boost/signals2.hpp>

#include <list>

class AntMap;
class AntHero;
class AntBoss;
class AntHouse;
class Node;

class AntPlayer {
public:
  AntPlayer(AntMap *pMap,const AGString &pName);
  virtual ~AntPlayer();
  
  void saveXML(Node &node) const;
  void loadXML(const Node &node);
  
  void move(float pTime);
  
  AGString getName() const;
  virtual void assignJob(AntBoss *boss)=0;
  
  void remove(AntBoss *boss);
  void add(AntBoss *boss);
  std::list<AntHouse*> getBuildings();
  std::list<AntHero*> getHeroes();
  std::list<AntBoss*> getBosses();
  
  virtual AGString xmlName()=0;

  boost::signals2::signal<void(AntPlayer*)> sigHeroesChanged;

private:
  
  void initBosses();
  
  AntMap *map;
  std::list<AntBoss*> bosses;
  std::list<AGString> bossNames;
  AGString name;
};

#endif
