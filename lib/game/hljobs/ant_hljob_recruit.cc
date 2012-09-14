#include "ant_hljob_recruit.h"
#include "ant_hero.h"
#include "map.h"
#include "formations/ant_formation_rest.h"
#include "rk_debug.h"

AntHlJobRecruit::AntHlJobRecruit ( AntBoss* pBoss, AntBoss* pTarget ) : AntHLJobMoving ( pBoss,pTarget->getEntity()->getPos2D(),3,true )

{
  targetId=pTarget->getID();
  fetchingStarted=false;
  mBasePos=AGVector2 ( -1,-1 );
  fetchingFinished=false;
  wantedMen=pTarget->getMenWithoutBoss().size() *pBoss->getEntity()->getAggression() /3;

}

void AntHlJobRecruit::doCollect()
{
  CTRACE;
  mBasePos=getHero()->getPos2D();
  fetchingStarted=true;
  AntBoss *myBoss=getBoss();
  std::vector< AntPerson* > myMen=myBoss->getMenWithoutBoss();
  if ( myMen.size() ==0 )
    myMen=myBoss->getMenWithBoss();
  std::vector< AntPerson* > otherMen=getTargetBoss()->getMenWithoutBoss();
  std::vector< AntPerson* > otherMenNear;
  AGVector2 heroPos=myBoss->getEntity()->getPos2D();
  for ( std::vector< AntPerson* >::iterator otherMenIterator=otherMen.begin(); otherMenIterator!=otherMen.end(); otherMenIterator++ )
    {
      if ( ( ( *otherMenIterator )->getPos2D()-heroPos ).length() <10 )
        {
          otherMenNear.push_back ( *otherMenIterator );
        }
    }

  for ( size_t i=0; i<myMen.size() && i<otherMenNear.size(); i++ )
    {
      if ( i>wantedMen )
        break;
      cdebug ( "fetching "<<myMen[i]->getID() <<" is "<<otherMen[i]->getID() );
      manMap[myMen[i]->getID()]=otherMen[i]->getID();
      myMen[i]->newMoveJob ( 0,otherMen[i],0.2 );

    }
}
void AntHlJobRecruit::checkPerson ( AntPerson* person )
{
  CTRACE;
  cdebug ( "recruit::check"<<person<<" "<<typeid ( person ).name() );
  if ( AntHLJobMoving::finished() )
    {
      CTRACE;
      AntBoss *boss=getBoss();
      if ( mBasePos.getX() <0 )
        mBasePos=boss->getEntity()->getPos2D();
      boss->setFormation ( new AntFormationRest ( boss ) );
      int personId=person->getID();
      if ( manMap.find ( personId ) !=manMap.end() )
        {
          CTRACE;
          int otherId=manMap[personId];
          // in map
          AntEntity *other=getMap()->getEntity ( otherId );
          AntMan *man=dynamic_cast<AntMan*> ( other );
          cdebug ( "other man:"<<other<<"  type:"<<typeid ( *other ).name() <<"  man:"<<man<<" id:"<<personId );
          if ( man )
            {
              AGVector2 otherPos=other->getPos2D();
              float distance= ( otherPos-person->getPos2D() ).length();
              cdebug ( "distance:"<<distance );
              if ( distance <1 )
                {

                  man->setBoss ( getBoss() );
                  manMap.erase ( personId );
                  wantedMen--;
                  fetchingFinished=true;
                }
            }

        }
      else
        {
          CTRACE;
          // rest
          AGVector2 sitPoint=boss->getFormation()->getPosition ( person,mBasePos );
          if ( ( person->getPos2D()-sitPoint ).length() <0.2 )
            {
              sit ( person );
            }
          else
            {
              person->newMoveJob ( 0,sitPoint,0 );
            }
        }
      if ( wantedMen>0 && boss->getRestingMenWithHero().size() ==boss->getMenWithBoss().size() )
        {
          doCollect();
        }


    }
  else
    {
      AntHLJobMoving::checkPerson ( person );
    }
}



AntBoss* AntHlJobRecruit::getTargetBoss()
{
  return getMap()->getBoss ( targetId );
}

AGVector2 AntHlJobRecruit::basePos()
{
  return mBasePos;
}

void AntHlJobRecruit::eventMoveFinished()
{
  doCollect();
}
bool AntHlJobRecruit::finished()
{
  return fetchingFinished;
}
