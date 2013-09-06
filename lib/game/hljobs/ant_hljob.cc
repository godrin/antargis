#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"
#include "ant_boss.h"
#include "ant_hero.h"
#include "ant_man.h"
#include "map.h"
#include <cmath>

AntHLJob::AntHLJob ( AntBoss* pBoss ) :mMap ( pBoss->getMap() )
{
  heroId=pBoss->getID();
}

AntBoss* AntHLJob::getBoss()
{

  AntBoss *boss=mMap->getBoss ( heroId );
  if ( !boss )
    throw std::string ( "boss to heroId " ) +AGString ( heroId ) +" not found";
  return boss;
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


void AntHLJob::sit ( AntPerson* man )
{
  AGVector2 formationPos=getBoss()->getFormation ( man,basePos() );

  AGVector2 diff= ( man->getPos2D()-formationPos );
  AntEntity *hero=getBossEntity();
  float dist=diff.length2();
  if ( dist<0.1 )
    {
      man->setDirection ( 180- ( hero->getPos2D()-man->getPos2D() ).normalized().getAngle().angle*180.0/M_PI );
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
        }
    }
  else
    {
      man->newMoveJob ( 0,formationPos,0 );
    }
}

AGVector2 AntHLJob::basePos()
{
  return getBossEntity()->getPos2D();
}

void AntHLJob::check ( AntHero* p )
{
  checkPerson ( p );
}

void AntHLJob::check ( AntMan* p )
{
  checkPerson ( p );
}

void AntHLJob::checkPerson ( AntPerson* p )
{
}
