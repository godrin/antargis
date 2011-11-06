/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * jobs.cc
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#include "jobs.h"
#include "entity.h"
#include "map.h"
#include <math.h>


#define WATER_MARK -0.2

Job::Job():priority(0),inited(false)
{
}

Job::Job(int p):priority(p),inited(true)
{}

bool Job::valid() const
{
  return inited;
}

void Job::move(AntEntity *,float ptime)
  {}

bool Job::operator<=(const Job &j) const
{
  return priority<=j.priority;
}

Job::~Job()
  {
  }

void Job::jobFinished(AntEntity *e)
  {
    e->sigJobFinished();
  }

bool Job::needsMorale() const
{
  return false;
}

void Job::saveXML(Node &pNode) const
{
  pNode.set("priority",AGString(priority));
}
void Job::loadXML(const Node &pNode)
  {
    if(pNode.get("priority").length())
      priority=pNode.get("priority").toInt();
    inited=true;
  }

AGString Job::xmlName() const
{
  return "antJob";
}


/************************************************************************
 * MoveJob
 ************************************************************************/

MoveJob::MoveJob():
  mTargetEntity(0)
  {
  }

MoveJob::MoveJob(int p,AntEntity *pTarget,float pNear,bool pRun):Job(p),mTargetEntity(pTarget),mNear(pNear),mRun(pRun)
{
  assert(mTargetEntity);
  m3d=false;
  // speed=70; // pixels per second
  //  runSpeed=100;
}

MoveJob::MoveJob(int p,const AGVector2 &pTarget,float pNear,bool pRun):Job(p),mTarget(pTarget),mNearTarget(pTarget),mTargetEntity(0),mNear(pNear),mRun(pRun)
{
  m3d=false;
  // speed=70; // pixels per second
  //  runSpeed=100;
}

MoveJob::MoveJob(int p,const AGVector3 &pTarget,float pNear,bool pRun):Job(p),mTarget3(pTarget),mTargetEntity(0),mNear(pNear),mRun(pRun)
{
  m3d=true;
  // speed=70; // pixels per second
  //  runSpeed=100;
}

MoveJob::~MoveJob()
  {
  }

// Jobs
void MoveJob::move(AntEntity *e,float ptime)
  {
    float aspeed;
    float speed=e->getSpeed();

    if(mTargetEntity)
      {
        mTarget=mTargetEntity->getPos2D();
        mTarget3=mTargetEntity->getPos3D();
      }

    mTarget=e->getMap()->truncPos(mTarget);

#ifdef ENABLE_RUNNING  
    float runSpeed=speed*1.3;

    if(mRun && e->getCondition()>0.0)
      {
        // decrease condition and if condition is zero - switch of running
        float newtime=e->decCondition(ptime);
        moveBy(e,ptime-newtime,runSpeed);// take same runSpeed always

        ptime=newtime;
      }
#endif
    aspeed=0.5*speed+0.5*e->getEnergy()*speed;
    assert(this);
    moveBy(e,ptime,aspeed); // use rest of time
    assert(this);

  }

AGVector2 MoveJob::getTargetPos2D() const
{
  return mTarget;
}


AGVector2 MoveJob::getDirection(const AntEntity *e) const
{
  return (mTarget-e->getPos2D()).normalized();
}


void MoveJob::moveBy(AntEntity *e,float ptime,float aspeed)
  {
    assert(e);
    float d0=e->getMap()->getPos(e->getPos2D())[2];

    if(d0<WATER_MARK && e->isOnGround())
      {
        e->eventHitWaterMark(true);
        return; // do nothing
      }
    else if(d0>WATER_MARK && e->isOnWater())
      {
        e->eventHitWaterMark(false);
        return; // do nothing
      }

    AGVector3 oldPos=e->getPos3D();

    if(m3d)
      {
        AGVector3 diff=e->getPos3D()-mTarget3;
        float norm=diff.length();

        if(norm-mNear>ptime*aspeed)
          {
            diff=diff.normalized();
            e->setDirection(-diff.getAngle().angle*180.0/M_PI);
            e->setPos(e->getPos3D()-diff*ptime*aspeed);
          }
        else
          {
            if(norm>mNear)
              e->setPos(mTarget3+diff.normalized()*mNear);
            e->setDirection(-diff.dim2().getAngle().angle*180.0/M_PI);
            jobFinished(e);
          }

      }
    else
      {
        AGVector2 diff=e->getPos2D()-mTarget;
        float norm=diff.length();

        float nearDist=(e->getPos2D()-mNearTarget).length();

        if(norm-mNear>ptime*aspeed && nearDist-mNear>ptime*aspeed)
          {
            diff=diff.normalized();
            e->setDirection(-diff.getAngle().angle*180.0/M_PI);
            e->setPos(e->getPos2D()-diff*ptime*aspeed);
          }
        else
          {
            if(norm>mNear)
              e->setPos(mTarget+diff.normalized()*mNear);
            e->setDirection(-diff.getAngle().angle*180.0/M_PI);
            
            jobFinished(e);
          }
      }


    float d1=e->getMap()->getPos(e->getPos2D())[2];
    /*  if(d0<WATER_MARK && d1>WATER_MARK)
    e->eventHitWaterMark(false);
  else if(d0>WATER_MARK && d1<WATER_MARK)
    e->eventHitWaterMark(true);
     */

    bool resetPos=false;

    if(d1<WATER_MARK && e->isOnGround())
      {
        if(!e->eventHitWaterMark(true))
          resetPos=true;
      }
    else if(d1>WATER_MARK && e->isOnWater())
      {
        if(!e->eventHitWaterMark(false))
          resetPos=true;
      }
    if(resetPos)
      e->setPos(oldPos);

  }

void MoveJob::saveXML(Node &pNode) const
{
  Job::saveXML(pNode);
  pNode.set("pos",mTarget.toString());
  pNode.set("near",AGString(mNear));
  pNode.set("run",mRun?"true":"false");
}
void MoveJob::loadXML(const Node &pNode)
  {
    Job::loadXML(pNode);
    if(pNode.get("pos").length())
      mTarget=AGVector2(pNode.get("pos"));
    if(pNode.get("near").length())
      mNear=pNode.get("near").toFloat();
    if(pNode.get("run").length())
      mRun=pNode.get("run")=="true";
  }
AGString MoveJob::xmlName() const
{
  return "moveJob";
}



/************************************************************************
 * FightJob
 ************************************************************************/

FightJob::FightJob():mTarget(0),mTargetID(-1)
{
}

FightJob::FightJob(int p,AntEntity *pTarget,float pDistance):Job(p),mTarget(pTarget)
{
  fightDistance=pDistance;
  moving=true;
}

FightJob::~FightJob()
  {
  }

bool FightJob::needsMorale() const
{
  return true;
}

void FightJob::move(AntEntity *e,float ptime)
  {
    assert(e);
    if(mTarget==0)
      {
        mTarget=e->getMap()->getEntity(mTargetID);
        if(!mTarget)
          {
            cdebug("Could not find id:"<<mTargetID);
          }
        assert(mTarget);
      }

    if(e->getEnergy()<0)
      throw std::runtime_error("Problem: fighting, but I'm alread dead!");

    if(mTarget->defeated())
      {
        // already died - so no sigDefeated
        e->eventHaveDefeated(mTarget);
        jobFinished(e);
        return; // early out
      }

    if(mTarget->getEnergy()<=0.0)
      {
        // already died - so no sigDefeated
        e->eventHaveDefeated(mTarget);
        jobFinished(e);
        return; // early out
      }
    else if(mTarget->getMorale()<0.1)
      {
        mTarget->sigDefeated();
        e->eventHaveDefeated(mTarget);
        jobFinished(e);
        return; // early out
      }
    // if target is too far away run there, otherwise fight
    AGVector2 diff=e->getPos2D()-mTarget->getPos2D();
    float norm=diff.length();
    float speed=e->getSpeed();


    e->setDirection(-diff.getAngle().angle*180.0/M_PI);

    if(norm-fightDistance>ptime*speed)
      {
        // run
        diff=diff.normalized();
        e->setPos(e->getPos2D()-diff*ptime*speed);
        //    e->setDirection(-diff.getAngle().angle*180.0/M_PI);
        if(!moving)
          {
            e->eventStartMoving();
            moving=true;
          }
      }
    else
      {
        // fight
        mTarget->decEnergy(ptime*e->getStrength()*e->getAggression()/mTarget->getDefense());
        mTarget->decMorale(ptime*e->getMoraleStrength()/mTarget->getDefense()); // FIXME: estimate this value
        mTarget->eventGotFight(e);

        e->incExperience(ptime*e->learnAmount);
        if(moving)
          {
            e->eventStartFighting();
            moving=false;
          }

      }
  }

void FightJob::saveXML(Node &pNode) const
{
  Job::saveXML(pNode);
  pNode.set("target",AGString(mTarget->getID()));
  pNode.set("distance",AGString(fightDistance));
  pNode.set("moving",moving?"true":"false");
}
void FightJob::loadXML(const Node &pNode)
  {
    Job::loadXML(pNode);
    if(pNode.get("target").length())
      {
        mTarget=0;
        mTargetID=pNode.get("target").toInt();
      }

    if(pNode.get("distance").length())
      fightDistance=pNode.get("distance").toFloat();
    if(pNode.get("moving").length())
      moving=pNode.get("moving")=="true";
  }
AGString FightJob::xmlName() const
{
  return "fightJob";
}


/***************************************************************************
 *
 ***************************************************************************/

FetchJob::FetchJob():
  mTarget(0)
  {
  }

FetchJob::FetchJob(int p,const AGVector2&pTarget,AGString what):MoveJob(p,pTarget,0.5),mWhat(what),mTarget(0)
{
}
FetchJob::FetchJob(int p,AntEntity *pTarget,AGString what):MoveJob(p,pTarget->getPos2D(),0.5),mWhat(what),mTarget(pTarget)
{
}


FetchJob::~FetchJob()
  {
  }
void FetchJob::move(AntEntity *e,float ptime)
  {
    MoveJob::move(e,ptime);
  }

void FetchJob::jobFinished(AntEntity *e)
  {
    e->resource.add(mWhat,1);
    e->resourceChanged();
    MoveJob::jobFinished(e);

    if(mTarget)
      {
        mTarget->resource.sub(mWhat,1);
        mTarget->resourceChanged();
      }
  }

void FetchJob::saveXML(Node &pNode) const
{
  MoveJob::saveXML(pNode);
  pNode.set("target",AGString(mTarget->getID()));
  pNode.set("what",AGString(mWhat));
}
void FetchJob::loadXML(const Node &pNode)
  {
    MoveJob::loadXML(pNode);
    if(pNode.get("target").length())
      {
        mTarget=0;
        mTargetID=pNode.get("target").toInt();
      }

    if(pNode.get("what").length())
      mWhat=pNode.get("what");

  }
AGString FetchJob::xmlName() const
{
  return "fetchJob";
}


/***************************************************************************
 *
 ***************************************************************************/

RestJob::RestJob():mTime(0),mWork(false)
{
}

RestJob::RestJob(float pTime,bool pWork):Job(0),mTime(pTime),mWork(pWork)
{
}
RestJob::~RestJob()
  {
  }
void RestJob::move(AntEntity *e,float ptime)
  {
    if(mWork)
      e->decMorale(std::min(ptime,mTime)/30);
    else
      {
        e->incMorale(std::min(ptime,mTime));
        e->heal(std::min(ptime,mTime));
      }
    mTime-=ptime;
    if(mTime<0)
      jobFinished(e);
  }

void RestJob::saveXML(Node &pNode) const
{
  Job::saveXML(pNode);

  pNode.set("time",AGString(mTime));
}
void RestJob::loadXML(const Node &pNode)
  {
    Job::loadXML(pNode);

    if(pNode.get("time").length())
      mTime=pNode.get("time").toFloat();
  }
AGString RestJob::xmlName() const
{
  return "restJob";
}

