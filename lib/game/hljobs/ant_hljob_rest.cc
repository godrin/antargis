#include "ant_formation_rest.h"
#include "ant_hero.h"
#include "ant_hljob_rest.h"
#include "ant_man.h"
#include <algorithm>
#include <cmath>

AntHLJobRest::AntHLJobRest(AntBoss* pBoss): AntHLJob(pBoss),mTime(0)
{
  initRestJob();
}

AntHLJobRest::AntHLJobRest(AntBoss* pBoss, float pTime): AntHLJob(pBoss),mTime(pTime)
{
  initRestJob();
}

void AntHLJobRest::initRestJob() {
  firstTime=true;
  spreadingThings=false;
  jobFinished=false;
  getBoss()->setFormation(new AntFormationRest(getBoss(),AntHero::FIRE_DISPLACE));
  assignAll();
}

void AntHLJobRest::checkPerson(AntPerson* person)
{
  AntHero *hero=dynamic_cast<AntHero*>(person);

  if(hero) {
    if (firstTime) {
      hero->newRestJob(mTime);
      hero->setFire(true);
      firstTime=false;
    } else {
      jobFinished=true;
    }
    eat(hero);
  } else {

    AntMan *man=dynamic_cast<AntMan*>(person);

    switch (man->getMode()) {
      case AntMan::REST_EAT: 
        {
          // men gather at the position of the hero
          if(moveTo(man,getBossEntity()->getPos2D())) {
            // reached hero's position
            spreadThings();
            eat(man);
            man->setMode(AntMan::REST_SIT);
          }
          break;
        }
      case AntMan::REST_SIT: 
        {
          // sit at man's position in rest formation
          if(sit(man)) {
            // really ready with sitting down
            if (man->getFood()<0.5) { 
              eat(man);
              if(heroHasFood()) {
                man->setMode(AntMan::REST_EAT);
              }
            }
          }
          break;
        }
      default:
        sit(man);
        break;

    }
  }
}

bool AntHLJobRest::heroHasFood()
{
  return getBossEntity()->resource.get("food")>1;
}

void AntHLJobRest::eat(AntPerson* man)
{
  if (man->getFood()<0.5) {
    if (man->resource.get("food")>0) {
      man->incFood(1);
      man->playSound("eat");
      man->resource.sub("food",1);
    } else if (heroHasFood()) {
      man->incFood(1);
      getBoss()->getEntity()->resource.sub("food",1);
    }
  }
}

void AntHLJobRest::spreadThings()
{
  if (!spreadingThings) {
    spreadingThings=true;

    spreadThing("sword");
    spreadThing("shield");
    spreadThing("bow");
    spreadThing("boat");

  }

  spreadFood();
}
void AntHLJobRest::spreadFood()
{
  float food=0;

  std::vector< AntPerson* > men=getMenWithBoss();

  // gather all food
  for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
    food+=(*i)->resource.get("food");
  }

  // use floor of quotient to spread food equally
  int min=(int)(food/men.size());

  for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
    (*i)->resource.set("food",min);
  }
  // spread rest on first in array (hero first)
  food-=min*men.size();
  for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
    if (food<=0)
      break;
    (*i)->resource.add("food",1);
    food-=1;
  }

}

class CompareResourceDesc {
  public:
    CompareResourceDesc(const AGString &pwhat):what(pwhat) {}
    bool operator()(AntEntity *a,AntEntity *b) {
      return a->resource.get(what)<b->resource.get(what);
    }
  private:
    AGString what;
};

void AntHLJobRest::spreadThing(const AGString& what)
{
  std::vector< AntPerson* > men=getMenWithoutBoss();
  float count=0;

  for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
    count+=(*i)->resource.get(what);
  }
  count+=getBossEntity()->resource.get(what);

  if (count>men.size()) {
    // give everyone 1 of these and put the rest on the hero
    for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
      (*i)->resource.set(what,1);
    }
    getBossEntity()->resource.set(what,count-men.size());
  } else {
    // now give to those who had a weapon and to hero (at first)
    std::vector<AntEntity*> men2;
    men2.push_back(getBossEntity());
    std::copy(men.begin(),men.end(),std::back_inserter(men2));
    std::sort(men2.begin(),men2.end(),CompareResourceDesc(what));
    for (std::vector<AntEntity*>::iterator i=men2.begin();i!=men2.end();i++) {
      (*i)->resource.set(what,count>0?1:0);
      count-=1;
    }

  }
}


AGVector2 AntHLJobRest::basePos()
{
  return getBossEntity()->getPos2D();
}

bool AntHLJobRest::finished()
{
  return jobFinished;
}


AGString AntHLJobRest::xmlName() const {
  return "hljobRest";
}
void AntHLJobRest::saveXML(Node &node) const {
  AntHLJob::saveXML(node);
  node.set("restTime",mTime);
  node.set("firstTime",firstTime);
  node.set("spreadingThings",spreadingThings);
  node.set("jobFinished",jobFinished);
}
void AntHLJobRest::loadXML(const Node &node) {
  AntHLJob::loadXML(node);
  mTime=node.get("restTime").toFloat();
  firstTime=node.get("firstTime").toBool();
  spreadingThings=node.get("spreadingThings").toBool();
  jobFinished=node.get("jobFinished").toBool();
}
