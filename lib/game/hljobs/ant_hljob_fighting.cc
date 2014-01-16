#include "ant_hljob_fighting.h"
#include "ant_hero.h"
#include "ant_formation_rest.h"
#include "ant_formation_singlepos.h"
#include "ant_house.h"
#include "map.h"

AntHlJobFighting::AntHlJobFighting(AntBoss* pBoss,AntBoss *pTarget, bool attacking): AntHLJobMoving(pBoss,pTarget->getEntity()->getPos2D(),5,true,!attacking),target(pTarget)
{
  CTRACE;
  state=START;
  mAttacking=attacking;
  if(!attacking) {
    dontMoveAnymore();
    startFighting();
  }
}


AGString AntHlJobFighting::xmlName() const {
  return "hljobFighting";
}
void AntHlJobFighting::saveXML(Node &node) const {
}
void AntHlJobFighting::loadXML(const Node &node){
}
bool AntHlJobFighting::checkPerson ( AntPerson* person ) {
  if(AntHLJobMoving::checkPerson(person)) {
    switch(state) {
      case START:
        startFighting();

        state=FIGHTING;
        break;
      case FIGHTING:
        cdebug("fight? "<<person);
        if(fight(person)) {
          cdebug("ready fighting");
          AntBoss *thisBoss=getBoss();
          if(!thisBoss)
            throw std::runtime_error("thisBoss is null in AntHlJobFighting::checkPerson");
          cdebug("fighting:Noone to fight anymore "<<thisBoss);
          // finished fighing

          if(fightingMenIDs.size()==0) {
            cdebug("LOST");
            state=LOST;
            reactOnLost();
          }
          auto targetFightJob=getTargetFightJob();
          if(targetFightJob && targetFightJob->fightingMenIDs.size()==0) {
            state=WON;
            cdebug("state=WON !");
            reactOnWon();
          } else {
            sit(person,startPos);
          }
          //          state=WON;
        } 
        break;
      case WON:
        cdebug("fighting:in won");
        sit(person,startPos);
        break;

      default:
        cdebug("fighting:default");
        break;
    }
    return true;
  }
  return false;
}
bool AntHlJobFighting::finished()
{
  return state==FINISHED;
}

void AntHlJobFighting::startFighting() {
  CTRACE;
  // save startPos for later resting
  startPos=getBossEntity()->getPos2D();
  auto men=getMenWithBoss();
  cdebug("MEN WITH BOS:"<<men.size());
  for(auto man:getMenWithBoss()) {
    cdebug("add fighting man:"<<man);
    man->setMode(AntPerson::FIGHTING);

    fightingMenIDs.insert(man->getID());
    if(mAttacking) {
      man->newRestJob(2);
      man->setMeshState("stand");
      cdebug("set restjob2:"<<man);
    }
    else
      man->newRestJob(0);
  }
  auto targetJob=target->getHlJob();

  if(!dynamic_cast<AntHlJobFighting*>(targetJob)) {
    CTRACE;
    target->setHlJob(new AntHlJobFighting(target,getBoss(),false));
  } else {
    CTRACE;
  }
  AntFormation *formation=0;
  auto thisBoss=getBoss();
  if(dynamic_cast<AntHouse*>(getBoss()))
    formation=new AntFormationSinglePos(thisBoss);
  else
    formation=new AntFormationRest ( thisBoss, AntHero::FIRE_DISPLACE) ;
  thisBoss->setFormation ( formation);
}

AntHlJobFighting *AntHlJobFighting::getTargetFightJob() {
  return dynamic_cast<AntHlJobFighting*>(target->getHlJob());
}

bool AntHlJobFighting::fight(AntPerson *person) {

  CTRACE;
  cdebug("MY HERO:"<<getBossEntity()->getName());
  if(fightingMenIDs.find(person->getID())==fightingMenIDs.end()) {
    cdebug("MAN READY:"<<person->getID()<<" size."<<fightingMenIDs.size());
    return true;
  }

  auto targetFightJob=getTargetFightJob();

  if(!targetFightJob) {
    if(!mAttacking) {
      getBoss()->setHlJob(0);
    } else {
      cdebug("ERROR: Target has no fight job anymore!");
    }
    return true;
  }

  // find next enemy
  auto enemyMen=targetFightJob->fightingMenIDs;
  cdebug("enemyMe size:"<<enemyMen.size());
  float minDist=1000.0f;
  AntPerson *found=0;

  for(auto manID:enemyMen) {
    auto man=dynamic_cast<AntPerson*>(getMap()->getEntity(manID));
    if(!man) {
      cdebug("this was no man:"<<manID);
      continue;
    }
    float dist=(person->getPos2D()-man->getPos2D()).length2();
    if(minDist>dist) {
      minDist=dist;
      found=man;
    }
  }
  if(found) {
    person->newFightJob(0,found);
  }
  else {
    cdebug("non found");
    return true;
  }

  return false;
}


void AntHlJobFighting::removeFightingperson(AntPerson *person) {
  CTRACE;
  fightingMenIDs.erase(person->getID());
  cdebug("fighting men size:"<<fightingMenIDs.size());
}

void AntHlJobFighting::reactOnLost() {
  getBoss()->setPlayer(target->getPlayer());
  getBoss()->setHlJob(0);
}
void AntHlJobFighting::reactOnWon() {
  target->setPlayer(getBoss()->getPlayer());
}

void AntHlJobFighting::eventJobDiscarded() {
  CTRACE;
  AntHLJobMoving::eventJobDiscarded();
  if(mAttacking) {
    CTRACE;
    target->setHlJob(0);
  }
}
