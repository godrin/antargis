#include "ant_hljob_fetching.h"
#include "entity.h"
#include <ant_man.h>
#include <ant_hero.h>
#include <ant_house.h>
#include <ag_rand.h>
#include <algorithm>
#include "map.h"

AntHLJobFetching::StockNeed::StockNeed(const std::string& r, float a,float cNeed):resource(r),amount(a),currentNeed(cNeed)
{

}


bool AntHLJobFetching::StockNeed::operator<(const AntHLJobFetching::StockNeed& o) const
{
  return amount<o.amount;
}


AntHLJobFetching::AntHLJobFetching(AntBoss* pBoss): AntHLJob(pBoss)
{
  mode=FETCH;
}
AGVector2 AntHLJobFetching::basePos()
{
  return getBossEntity()->getPos2D();
}
void AntHLJobFetching::checkPerson(AntPerson *person)
{
  AntMan *man=dynamic_cast<AntMan*>(person);
  if(man)
    if (atHome(man))  {
      menAtHome.insert(man);
      if (checkFood(man)) {
        man->newRestJob(3);
        return;
      }
      if (man->getMode()==AntPerson::REST_SIT) {
        man->setMode(AntPerson::READY);
        man->setMeshState("stand");
        // is home:
        // 1) take everything from inventory
        getBossEntity()->resource.takeAll(man->resource);
        getBossEntity()->resourceChanged();
        // 2) give job
        std::vector<StockNeed> need=neededStock();
        fetchForStock(need,man);
      } else {
        man->setMode(AntPerson::REST_SIT);
        man->newRestJob(5+agRand(1.0f));
      }
    } else if (man->getMode()==AntPerson::FETCHING)  {
      // check if target is somehow to far away
      if ((man->getTargetPos2D()-man->getPos2D()).length()>2) {
        man->setMode(AntPerson::HOMING);
        man->setMeshState(man->getFetchResource());
        man->newMoveJob(0,getBossEntity()->getPos2D(),0);
        return;
      }
      man->digResource(man->getFetchResource());
      man->setMode(AntPerson::DIGGING);
    } else if (man->getMode()==AntPerson::DIGGING) {
      // digging ready - take home
      man->newMoveJob(0,getBossEntity()->getPos2D(),0);
      //res=e.getMode.gsub(/.* /,"")
      man->setMode(AntPerson::HOMING);
      man->setMeshState(man->getFetchResource());
      AGString res=man->getFetchResource();
      // take resource
      if (!man->getTarget()) // FIXME: error while loading
      {
        return;
      }
      man->collectResource(res);
      float amount=std::min(man->getTarget()->resource.get(res),man->canCarry());
      man->getTarget()->resource.sub(res,amount);
      man->getTarget()->resourceChanged();
      man->resource.add(res,amount);
    } else if (man->getMode()==AntPerson::HOMING) {
      man->newRestJob(1); // always rest a little
      if(man->getMeshState()=="stand")
        man->setMeshState("walk");
      man->setMode(AntPerson::READY);
    } else {
      // is anywhere - come home
      man->newMoveJob(0,getBossEntity()->getPos2D(),0);
    }
  AntHero *hero=dynamic_cast<AntHero*>(person);
  if(hero)
    hero->newRestJob(10);
}

bool AntHLJobFetching::finished()
{
  return true;
}

bool AntHLJobFetching::atHome(AntMan* man)
{
  AGVector2 distV=man->getPos2D()-basePos();
  float n=distV.length2();
  return n<1;
}

bool AntHLJobFetching::checkFood(AntMan* man)
{
  if (man->getFood()<0.5) {
    // eat
    if (getBossEntity()->resource.get("food")>1) {
      man->incFood(1);
      getBossEntity()->resource.sub("food",1);
      return true;
    }
  }
  return false;
}


void AntHLJobFetching::fetchForStock(std::vector<StockNeed> needed, AntMan* man)
{
  std::vector<StockNeed> needmap=neededStock();
  std::set<StockNeed> sortedNeed;

  std::random_shuffle(needmap.begin(),needmap.end());

  for (std::vector<StockNeed>::iterator i=needmap.begin();i!=needmap.end();i++) {
    sortedNeed.insert(StockNeed(i->resource,i->amount,i->amount-getBossEntity()->resource.get(i->resource)));

  }
  float factor=2;
  if (mode==REST) {
    factor=1;
  }
  for (std::set<StockNeed>::iterator current=sortedNeed.begin();current!=sortedNeed.end();current++) {
    std::string resource=current->resource;
    float amount=current->amount;
    float need=current->currentNeed;

    if (need>0 || (need>-amount*0.5 && mode==FETCH)) {
      mode=FETCH;
      AntEntity *tent=0;

      if (resource=="food") {
        tent=getBossEntity()->getMap()->getNext(getBossEntity(),resource,11); // don't take away food
      } else {
        tent=getBossEntity()->getMap()->getNext(getBossEntity(),resource,1);
      }
      bool okToHarvest=true;
      if (tent) {
        AntHouse *house=dynamic_cast<AntHouse*>(tent);
        if (house) {
          if (house->getPlayer() && house->getPlayer()!=getBoss()->getPlayer()) {
            okToHarvest=false;
          }
        }

        if (okToHarvest) {
          man->newMoveJob(0,tent,0.5);
          man->setMode(AntPerson::FETCHING);
          man->setMeshState("walk");
          man->setFetchResource(resource);
          menAtHome.erase(man);
          man->setVisible(true);
          return;
        }
      } else {
        cdebug("no entity found for:"<<resource);
      }
    }
  }

  mode=REST;
  man->newRestJob(5); // do nothing for a longer time
}


std::vector< AntHLJobFetching::StockNeed > AntHLJobFetching::neededStock()
{
  std::vector< AntHLJobFetching::StockNeed > a;
  a.push_back(StockNeed("wood",15,0));
  a.push_back(StockNeed("stone",15,0));
  a.push_back(StockNeed("food",15,0));
  return a;
}
