#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"
#include "ant_boss.h"
#include "ant_hero.h"
#include "ant_man.h"
#include "map.h"
#include "ant_formation.h"
#include <cmath>

AntHLJob::AntHLJob ( AntBoss* pBoss ) :mMap ( pBoss->getMap() )
{
  if(!pBoss)
    throw std::runtime_error("pBoss is null in AntHLJob::AntHLJob");
#ifdef ANTHLJOB_SAVE_ONLY_ID
  heroId=pBoss->getID();
#else
  mBoss=pBoss;
#endif
}

AntBoss* AntHLJob::getBoss()
{

#ifdef ANTHLJOB_SAVE_ONLY_ID
  AntBoss *boss=mMap->getBoss ( heroId );
  if ( !boss )
    throw std::runtime_error ( AGString("boss to heroId " ) +AGString ( heroId ) +" not found");
  return boss;
#else
  return mBoss;
#endif
}

std::vector< AntPerson* > AntHLJob::getMenWithBoss()
{
  return getBoss()->getMenWithBoss();
}

std::vector< AntPerson* > AntHLJob::getMenWithoutBoss()
{
  return getBoss()->getMenWithoutBoss();
}



AntEntity* AntHLJob::getBossEntity()
{
  return getBoss()->getEntity();
}

AntMap* AntHLJob::getMap()
{
  return getBossEntity()->getMap();
}
/**
  \return bool true, if sitting is done
  */
bool AntHLJob::sit ( AntPerson* man, const AGVector2 &pBasePos )
{
  AGVector2 bp;
  if(pBasePos.getX()<0) 
    bp=basePos();
  else
    bp=pBasePos;
  AGVector2 formationPos=getBoss()->getFormation ( man,bp );

  AGVector2 diff= ( man->getPos2D()-formationPos );
  //AntEntity *hero=getBossEntity();
  float dist=diff.length2();
  if ( dist<0.1 )
  {
    man->setMode(AntPerson::REST_SIT);
    man->setDirection ( 180- ( bp-man->getPos2D() ).normalized().getAngle().angle*180.0/M_PI );
    if ( man->getPos3D().getZ() <0 ) // under water
    {
      man->newRestJob ( 5 );
      man->setMeshState ( "stand" );

    }
    else if ( ! ( man->getMeshState() =="sitdown" || man->getMeshState() =="sit" ) )
    {
      man->sitDown();
    }
    else
    {
      man->newRestJob ( 5 );
      man->setMeshState ( "sit" );
      man->setMode(AntPerson::REST_SIT);
      return true;
    }
  }
  else
  {
    man->setMeshState("walk");
    man->newMoveJob ( 0,formationPos,0 );
  }
  return false;
}

bool AntHLJob::moveTo(AntPerson *man, const AGVector2 &pPos, bool withFormation) {
  CTRACE;
  AGVector2 pos=pPos;
  if(withFormation && !dynamic_cast<AntHero*>(man)) {
    pos=getBoss()->getFormation()->getPosition(man,pos);
  }
  AGVector2 diff=man->getPos2D()-pos;
  if(diff.length2()<0.1) {
    return true;
  }

  switch(man->getMode()) {
    case AntPerson::REST_SIT:
      man->standUp();
      man->setMode(AntPerson::FORMAT);
      break;
    default:
      man->setMeshState("walk");
      man->newMoveJob(0,pos,0);
      break;

  }

  return false;
}

AGVector2 AntHLJob::basePos()
{
  return getBossEntity()->getPos2D();
}

bool AntHLJob::checkPerson ( AntPerson* p )
{
  return true;
}

void AntHLJob::assignAll() {
  CTRACE;
  for(auto entityIter:getMenWithBoss()) {
    checkPerson(entityIter);
  }
}

void AntHLJob::saveXML(Node &node) const {
  // bossID shouldn't be saved - it's derived from boss on object creation
}

void AntHLJob::loadXML(const Node &node) {
}

bool AntHLJob::fireBurning() const {
  return true;
}
  
void AntHLJob::eventJobDiscarded() {
}

