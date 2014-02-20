#include "ant_hljob_fetching.h"
#include "entity.h"
#include "map.h"
#include <ag_rand.h>
#include <algorithm>
#include <ant_hero.h>
#include <ant_house.h>
#include <ant_man.h>

AntStockNeed::AntStockNeed(const std::string& r, float a,float cNeed):resource(r),amount(a),currentNeed(cNeed)
{

}


bool AntStockNeed::operator<(const AntStockNeed& o) const
{
  return amount<o.amount;
}


AntHLJobFetching::AntHLJobFetching(AntBoss* pBoss): AntHLJob(pBoss)
{
  CTRACE;
  mode=FETCH;
/*  for(auto man:getMenWithoutBoss()) {
    checkPerson(man);
  }*/
}
AGVector2 AntHLJobFetching::basePos()
{
  return getBossEntity()->getPos2D();
}
bool AntHLJobFetching::checkPerson(AntPerson *person)
{
  CTRACE;
  cdebug("checkPerson:"<<person);
  AntMan *man=dynamic_cast<AntMan*>(person);
  if(man) {
    if (atHome(man))  {
      menAtHome.insert(man);
      if (checkFood(man)) {
        man->newRestJob(3);
        return false;
      }
      if (man->getMode()==AntPerson::REST_SIT) {
        man->setMode(AntPerson::READY);
        man->setMeshState("stand");
        // is home:
        // 1) take everything from inventory
        getBossEntity()->resource.takeAll(man->resource);
        getBossEntity()->resourceChanged();
        // 2) give job
        std::vector<AntStockNeed> need=neededStock();
        fetchForStock(need,man);
      } else {
        man->setMode(AntPerson::REST_SIT);
        man->newRestJob(5+agRand(1.0f));
      }
    } else if (man->getMode()==AntPerson::FETCHING)  {
      CTRACE;
      cdebug("TAKING:"<<man->getTargetByID());
      auto *ent=man->getTargetByID();
      cdebug("target:"<<ent->getPos2D()<<" man:"<<man->getPos2D());
      // check if target is somehow to far away
      if ((man->getTargetPos2D()-man->getPos2D()).length()>2) {
        man->setMode(AntPerson::HOMING);
        man->setMeshState(man->getFetchResource());
        man->newMoveJob(0,getBossEntity()->getPos2D(),0);
        return false;
      }
      cdebug("DIG resource");
      man->setMeshState("walk");
      man->digResource(man->getFetchResource());
      man->setMode(AntPerson::DIGGING);
    } else if (man->getMode()==AntPerson::DIGGING) {
      cdebug("was diging");
      // digging ready - take home
      man->newMoveJob(0,getBossEntity()->getPos2D(),0);
      //res=e.getMode.gsub(/.* /,"")
      man->setMode(AntPerson::HOMING);
      AGString res=man->getFetchResource();
      if(res!="")
        man->setMeshState(man->getFetchResource());
      else
        man->setMeshState("walk");
      // take resource
      if (!man->getTargetByID()) // FIXME: error while loading
      {
        cdebug("No target defined ?");
        return false;
      }
      man->collectResource(res);
      auto target=man->getTargetByID();
      float amount=std::min(target->resource.get(res),man->canCarry());
      target->resource.sub(res,amount);
      target->resourceChanged();
      cdebug("GOT "<<amount<<" of "<<res<<" from "<<target);
      man->resource.add(res,amount);
    } else if (man->getMode()==AntPerson::HOMING) {
      man->newRestJob(1); // always rest a little
      //if(man->getMeshState()=="stand")
      man->setMeshState("walk");
      man->setMode(AntPerson::READY);
    } else {
      // is anywhere - come home
      man->newMoveJob(0,getBossEntity()->getPos2D(),0);
      man->setMeshState("walk");
    }
  }
  AntHero *hero=dynamic_cast<AntHero*>(person);
  if(hero)
    hero->newRestJob(10);
  return false;
}

bool AntHLJobFetching::finished()
{
  return false;
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


void AntHLJobFetching::fetchForStock(std::vector<AntStockNeed> needed, AntMan* man)
{
  std::vector<AntStockNeed> needmap=neededStock();
  std::set<AntStockNeed> sortedNeed;

  std::random_shuffle(needmap.begin(),needmap.end());

  for (std::vector<AntStockNeed>::iterator i=needmap.begin();i!=needmap.end();i++) {
    sortedNeed.insert(AntStockNeed(i->resource,i->amount,i->amount-getBossEntity()->resource.get(i->resource)));
  }
  float factor=2;
  if (mode==REST) {
    factor=1;
  }
  for (std::set<AntStockNeed>::iterator current=sortedNeed.begin();current!=sortedNeed.end();current++) {
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
          CTRACE;
          cdebug("HARVEST from house ????????????????????????????"<<house<<"  "<<getBossEntity()<<"  "<<getBoss()<<" ent:"<<tent<<"  "<<typeid(*tent).name()<<" what:"<<resource);
          man->newMoveJob(0,tent,0.5);
          cdebug("HARV POS:"<<tent->getPos2D()<<"  "<<man->getPos2D());
          man->setMode(AntPerson::FETCHING);
          man->setMeshState("walk");
          man->setFetchResource(resource);
          man->setTargetID(tent->getID());
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


std::vector< AntStockNeed > AntHLJobFetching::neededStock()
{
  return getBoss()->neededStock();
}
    
AGString AntHLJobFetching::xmlName() const {
  return "hljobFetching";
}

void AntHLJobFetching::saveXML(Node &node) const {
  AntHLJob::saveXML(node);
  node.set("fetchMode",mode);
}
void AntHLJobFetching::loadXML(const Node &node) {
  AntHLJob::loadXML(node);
  mode=(MODE)node.get("fetchMode").toInt();
}
bool AntHLJobFetching::startTogether() const {
  return true;
}
