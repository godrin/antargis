#include "ant_player.h"
#include <algorithm>
#include <ag_xml.h>
#include <ant_hero.h>
#include <ant_house.h>
#include <map.h>

AntPlayer::AntPlayer(AntMap* pMap, const AGString& pName):map(pMap),
        name(pName)
{

}

AntPlayer::~AntPlayer()
{
    for (std::list<AntBoss*>::iterator i=bosses.begin();i!=bosses.end();i++) {
        (*i)->setPlayer(0);
    }
}


void AntPlayer::add(AntBoss* boss)
{
    bosses.push_back(boss);
    sigHeroesChanged(this);
}

void AntPlayer::remove(AntBoss* boss)
{
    bosses.remove(boss);
    sigHeroesChanged(this);
}

AGString AntPlayer::getName() const
{
    return name;
}

std::list< AntBoss* > AntPlayer::getBosses()
{
    initBosses();
    return bosses;
}

std::list< AntHouse* > AntPlayer::getBuildings()
{
    initBosses();
    std::list<AntHouse*> hlist;
    for (std::list<AntBoss*>::iterator i=bosses.begin();i!=bosses.end();i++) {
        AntHouse *h=dynamic_cast<AntHouse*>(*i);
        if (h)
            hlist.push_back(h);
    }
    return hlist;
}

std::list< AntHero* > AntPlayer::getHeroes()
{
    initBosses();
    std::list<AntHero*> hlist;
    for (std::list<AntBoss*>::iterator i=bosses.begin();i!=bosses.end();i++) {
        AntHero *h=dynamic_cast<AntHero*>(*i);
        if (h)
            hlist.push_back(h);
    }
    return hlist;
}

void AntPlayer::loadXML(const Node& node)
{
    name=node.get("name");
    Node::NodeVector children=node.getChildren("hero");
    for (Node::NodeVector::iterator childIterator=children.begin();
            childIterator!=children.end();childIterator++) {
        bossNames.push_back((*childIterator)->get("name"));
    }

    for(auto child:node.getChildren("house")) {
      bossNames.push_back(child->get("name"));
    }
}

void AntPlayer::move(float pTime)
{
  initBosses();
}
void AntPlayer::initBosses() {
  std::list<AGString> notFound;
  bool changed=false;
  for (std::list<AGString>::iterator i=bossNames.begin();i!=bossNames.end();i++) {
    AntEntity *e=map->getByName(*i);
    if (e) {
      AntBoss *b=dynamic_cast<AntBoss*>(e);
      if (b) {
        bosses.push_back(b);
        b->setPlayer(this);
        changed=true;
      } else {
        cdebug("BOSS NOT FOUND 1:"<<*i);
        notFound.push_back(*i);
      }
    }
    else {
      cdebug("BOSS NOT FOUND 2:"<<*i);
      notFound.push_back(*i);
    }
  }
  bossNames=notFound;
  if(changed)
    sigHeroesChanged(this);
}

void AntPlayer::saveXML(Node& node) const
{
  node.set("name",name);
  for (std::list<AntBoss*>::const_iterator i=bosses.begin();i!=bosses.end();i++) {
    Node &n=node.addChild("hero");
    n.set("name",(*i)->getEntity()->getName());
  }
}
