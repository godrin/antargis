/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * entity.cc
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

#include "entity.h"
#include "map.h"
#include "rk_debug.h"
#include "jobs.h"
#include "mesh.h"
#include "anim_mesh.h"

#include <ag_mixer.h>

AntEntity::AntEntity(AntMap *pMap):mMap(pMap),mPos(0,0,0)
  {
    assert(mMap);
  }

void AntEntity::init()
  {
    mID=getMap()->getNewID();

    mShowOnMinimap=false;
    onGround=true;
    onWater=false;

    mJob=0;

    mVirtualY=40;

    mEnergy=1.0;
    mHealSpeed=0.07;

    mMorale=1.0;
    mMoraleHeal=0.05;

    mMoveSpeed=2;

    mAggression=1;

    //  mSurface=0;
    //esh=0;
    mDir=0;

    mStrength=0.02;
    mMoraleStrength=0.04;

    mFood=1.0;
    mHunger=0.006;
    mHungerHitEnergy=0.02;
    mHungerHitMorale=0.14;

    mMoraleFlag=true;

    mDefense=1.0;
    //  mDirNum=1;

    mMinimapSize=2;

    experience=0;
    learnAmount=0.1;

    mDefeated=false;
  }


AntEntity::~AntEntity() throw()
  {
    CTRACE;
    //#error called several times
    for(Meshes::iterator i=mMeshes.begin();i!=mMeshes.end();i++)
      checkedDelete(*i);
    mMeshes.clear();
    checkedDelete(mJob);
    removeOldJobs();
  }

void AntEntity::resourceChanged()
  {
  }

void AntEntity::saveXML(Node &node) const
{
  Node &child=node.addChild("position");
  mPos.saveXML(child);
  node.set("energy",AGString(mEnergy));
  node.set("healSpeed",AGString(mHealSpeed));
  node.set("onGround",AGString(onGround));
  node.set("onWater",AGString(onWater));
  //    node.set("entityID",AGString(getID()));
  node.set("name",getName());
  //    node.set("classtype",getType());
  node.set("morale",AGString(mMorale));
  node.set("aggression",AGString(mAggression));
  node.set("food",AGString(mFood));
  node.set("id",AGString(mID));
  node.set("exp",AGString(experience));
  node.set("learnAmount",AGString(learnAmount));

  node.set("defeated",AGString(mDefeated));

  Node &res=node.addChild("resource");
  resource.saveXML(res);

  if(mJob)
    {
      Node &j=node.addChild("job");
      j.set("type",mJob->xmlName());
      mJob->saveXML(j);
    }

}
void AntEntity::loadXML(const Node &node)
  {
    mEnergy=node.get("energy").toFloat();
    mHealSpeed=node.get("healSpeed").toFloat();
    onGround=node.get("onGround").toBool();
    onWater=node.get("onWater").toBool();

    mDefeated=node.get("defeated").toBool();
    //  assert(onGround);
    Node::NodeVector v=node.getChildren("position");
    Node::const_iterator i=v.begin();
    for(;i!=v.end();i++)
      mPos.loadXML(**i);

    AGString entID=node.get("entityID");
    if(entID.length()>0)
      mID=entID.toInt();
    else
      mID=node.get("id").toInt();
    getMap()->useID(mID);
    if(node.get("morale")!="")
      mMorale=node.get("morale").toFloat();
    else
      mMorale=1.0f;
    mAggression=node.get("aggression").toFloat();
    mAggression=std::min(3,std::max(1,int(mAggression))); //1,2,3 nothing else

    if(node.get("food")!="")
      mFood=node.get("food").toFloat();
    if(node.get("hunger")!="")
      mHunger=node.get("hunger").toFloat();

    setName(node.get("name"));
    //  setType(node.get("classtype"));

    if(node.get("exp")!="")
      experience=node.get("exp").toFloat();
    if(node.get("learnAmount")!="")
      learnAmount=node.get("learnAmount").toFloat();

    Node::NodeVector v2=node.getChildren("resource");
    if(v2.size()>0)
      resource.loadXML(v2[0]);

    v2=node.getChildren("job");
    if(v2.size()>0)
      {
        loadJob(v2[0]);
      }
    resourceChanged();
  }

void AntEntity::loadJob(const Node &pNode)
  {
    AGString t=pNode.get("type");
    if(t=="restJob")
      mJob=new RestJob;
    else if(t=="fightJob")
      mJob=new FightJob;
    else if(t=="moveJob")
      mJob=new MoveJob;
    else if(t=="fetchJob")
      mJob=new FetchJob;
    if(mJob)
      mJob->loadXML(pNode);
  }


AGVector3 AntEntity::getPos3D() const
{
  return mPos;
}
AGVector2 AntEntity::getPos2D() const
{
  return AGVector2(mPos[0],mPos[1]);
}

void AntEntity::updatePos(const AGVector3 &p)
  {
    if(mMeshes.size()==1)
      {
        mMeshes.front()->setPos(p);
        return;
      }
    for(Meshes::iterator i=mMeshes.begin();i!=mMeshes.end();i++)
      {
        (*i)->setPos(p+mMeshPos[*i]);
        (*i)->setRotation(mDir);
      }

  }

void AntEntity::setPos(const AGVector3 &p)
  {
    mPos=p;
    updatePos(p);
  }
void AntEntity::setPos(const AGVector2 &pp)
  {
    AGVector2 p=getMap()->truncPos(pp); // make sure, that position is ok
    if(onGround)
      mPos=getMap()->getPos(p);
    else if(onWater)
      mPos=AGVector3(p,0);
    else
      {
        mPos=AGVector3(p,mPos[2]);
      }
    updatePos(mPos);
  }

void AntEntity::delJob()
  {
    setJob(0);
  }

void AntEntity::setJob(Job *pJob)
  {
    //  assert(pJob);
    if(mJob)
      {
        if(pJob)
          {
            if((*mJob)<=(*pJob))
              {
                mJobFinished.push_back(mJob);
              }
            else
              {
                mJobFinished.push_back(mJob);
              }
          }
        else
          mJobFinished.push_back(mJob);
      }
    mJob=0;
    if(mEnergy>=0.0) // do job anyways
      {
        if(!pJob)
          mJob=pJob;
        else
          {
            if(mMorale>0.1 || !pJob->needsMorale()) // at least 10% morale
              mJob=pJob;
            else
              mJobFinished.push_back(pJob);
          }
      }
    else
      {
        if(pJob)
          mJobFinished.push_back(pJob);
      }
    if(mJob)
      eventGotNewJob();
  }

void AntEntity::eventGotNewJob()
  {
  }

void AntEntity::removeOldJobs()
  {
    std::list<Job*>::iterator i=mJobFinished.begin();
    for(;i!=mJobFinished.end();i++)
      checkedDelete(*i);
    mJobFinished.clear();
  }


/** do anything in given time frame */
void AntEntity::move(float pTime)
  {
    if(mJobFinished.size() || mEnergy==0.0)
      {
        removeOldJobs();
      }
    else if(mEnergy>0.0)
      {
        starve(pTime);

        if(mMorale<=0.1)
          if(mJob)
            {
              assert(mJob->valid());
              if(mJob->needsMorale())
                setJob(0);// kill job
            }
        if(!isStarving())
          {
            mEnergy+=pTime*getHealSpeed()*0.2; // very slow healing when doin something
            if(mEnergy>1.0)
              mEnergy=1.0;
          }
      }
    if(mJob)
      {
        assert(mJob->valid());
        mJob->move(this,pTime);
      }
    else
      eventNoJob();

  }

void AntEntity::heal(float pTime)
  {
    if(!isStarving())
      {
        mEnergy+=pTime*getHealSpeed()*0.8; // only rest til 1.0
        if(mEnergy>1.0)
          mEnergy=1.0;
      }
  }

void AntEntity::incMorale(float pTime)
  {
    if(!isStarving())
      {
        mMorale+=pTime*mMoraleHeal;
        if(mMorale>1.0)
          mMorale=1.0;
        if(mMorale>0.5)
          mDefeated=false;
      }
  }




void AntEntity::eventJobFinished()
  {
  }

void AntEntity::eventNoJob()
  {
  }


void AntEntity::eventMapChanged()
  {
    setPos(mPos);
    /*
  if(onGround)
    mPos=getMap()->getPos(AGVector2(getPos2D()));
  else if(onWater)
    mPos=AGVector3(getPos2D()[0],getPos2D()[1],0);
     */
    updatePos(mPos);
  }

void AntEntity::setMesh(SceneNode *m)
  {
    // clear meshes from scene
    for(Meshes::iterator i=mMeshes.begin();i!=mMeshes.end();i++)
      checkedDelete(*i);

    mMeshes.clear();
    mMeshPos.clear();
    if(m)
      {
        AnimMesh *mesh=dynamic_cast<AnimMesh*>(m);
        if(mesh)
          mesh->setEntity(this);
        mMeshes.push_back(m);

        updatePos(mPos);
      }
  }

void AntEntity::addMesh(SceneNode *m,const AGVector3 &v)
  {
    if(m)
      {
        AnimMesh *mesh=dynamic_cast<AnimMesh*>(m);
        if(mesh)
          mesh->setEntity(this);
        mMeshes.push_back(m);
        mMeshPos.insert(std::make_pair(m,v));

        updatePos(mPos);
      }
  }


AntEntity::Meshes AntEntity::getMesh()
  {
    return mMeshes;
  }

SceneNode *AntEntity::getFirstMesh()
  {
    assert(mMeshes.size()>0);
    return mMeshes.front();
  }



int AntEntity::getID() const
{
  return mID;
}

AGString AntEntity::xmlName() const
{

  
  return "antEntity";
}
std::string AntEntity::xmlName2() const
{
  return "antEntity";
}
AGString AntEntity::xmlName3() const
{
  return "antEntity";
}

float AntEntity::getDirection() const
{
  return mDir;
}


void AntEntity::setDirection(float dir)
  {
    mDir=dir;

    if(mMeshes.size())
      {
        SceneNode *m=mMeshes.front();
        if(m)
          m->setRotation(dir);
      }
  }

void AntEntity::setVisible(bool v)
  {
    for(Meshes::iterator i=mMeshes.begin();i!=mMeshes.end();i++)
      (*i)->setVisible(v);
  }




void AntEntity::newRestJob(float pTime,bool pWork)
  {
    setJob(new RestJob(pTime,pWork));
  }
void AntEntity::newFetchJob(int p,AGVector2 &pTarget,const AGString &what)
  {
    setJob(new FetchJob(p,pTarget,what));
  }
void AntEntity::newFetchJob(int p,AntEntity *pTarget,const AGString &what)
  {
    setJob(new FetchJob(p,pTarget,what));
  }
void AntEntity::newMoveJob(int p,const AGVector2 &pTarget,float pnear)
  {
    setJob(new MoveJob(p,pTarget,pnear));
  }
void AntEntity::newMoveJob(int p,const AGVector3 &pTarget,float pnear)
  {
    setJob(new MoveJob(p,pTarget,pnear));
  }

void AntEntity::newMoveJob(int p,AntEntity *pTarget,float pnear)
  {
    setJob(new MoveJob(p,pTarget,pnear));
  }


void AntEntity::newFightJob(int p,AntEntity *target,float distance)
  {
    if(!canFight())
      return;
    if(mJob)
      {
        FightJob *f=dynamic_cast<FightJob*>(mJob);
        if(f)
          if(f->getTarget()==target)
            return;
      }
    setJob(new FightJob(p,target,distance));
  }

void AntEntity::setSpeed(float f)
  {
    mMoveSpeed=f;
  }
float AntEntity::getSpeed() const
{
  return mMoveSpeed;
}

void AntEntity::setHealSpeed(float f)
  {
    mHealSpeed=f;
  }

void AntEntity::setMoraleHealSpeed(float f)
  {
    mMoraleHeal=f;
  }

void AntEntity::setMoraleFlag(bool f)
  {
    mMoraleFlag=f;
  }


void AntEntity::decEnergy(float amount)
  {
    mEnergy-=amount;
    if(mEnergy<0.0)
      {
        mEnergy=0.0;
        eventDie();
      }
  }

bool AntEntity::hasJob() const
{
  return mJob;
}

AGString AntEntity::getJobName() const
{
  if(mJob)
    return mJob->xmlName();
  return "";
}


float AntEntity::getHealSpeed() const
{
  return mHealSpeed;
}


void AntEntity::decMorale(float amount)
  {
    if(!mMoraleFlag)
      return;
    mMorale-=amount;
    if(mMorale<0.0)
      {
        mMorale=0.0;
        mDefeated=true;
        eventMoraleLow();
      }
  }


void AntEntity::eventDie()
  {
    sigJobFinished();
  }

float AntEntity::getMorale() const
{
  return mMorale;
}
float AntEntity::getEnergy() const
{
  return mEnergy;
}

void AntEntity::setAggression(float agg)
  {
    mAggression=agg;
  }
float AntEntity::getAggression() const
{
  return mAggression;
}

void AntEntity::eventGotFight(AntEntity*pOther)
  {
  }

void AntEntity::eventDefeated()
  {
  }

void AntEntity::sigDefeated()
  {
    if(!mMoraleFlag)
      return;
    mMorale=-0.1; // really deep morale
    mDefeated=true;
    // this prevents other fight-jobs from not being discarded
    eventDefeated();
  }



void AntEntity::setName(const AGString &pName)
  {
    mName=pName;
  }
AGString AntEntity::getName() const
{
  return mName;
}

/*void AntEntity::setType(const AGString &pType)
{
  mType=pType;
}
AGString AntEntity::getType() const
{
  return mType;
  }*/

AGRect2 AntEntity::getRect() const
{
  // FIXME: exchange this with something suitable
  return AGRect2(mPos[0]-0.1, mPos[1]-0.1, 0.2, 0.2);
}




void AntEntity::clear() throw()
  {
    clearMeshes();
  }


void AntEntity::clearMeshes()
  {
    for(Meshes::iterator i=mMeshes.begin();i!=mMeshes.end();i++)
      {
        checkedDelete(*i);
      }
    mMeshes.clear();
  }

void AntEntity::animationEvent(const AGString &pName)
  {
  }

void AntEntity::setMinimapColor(const AGColor &c)
  {
    mMinimapColor=c;
    mShowOnMinimap=true;
  }

AGColor AntEntity::getMinimapColor() const
{
  return mMinimapColor;
}

bool AntEntity::showOnMinimap() const
{
  return mShowOnMinimap;
}

void AntEntity::setStrength(float f)
  {
    mStrength=f;
  }
float AntEntity::getStrength() const
{
  return mStrength;
}
void AntEntity::setMoraleStrength(float f)
  {
    mMoraleStrength=f;
  }
float AntEntity::getMoraleStrength() const
{
  return mMoraleStrength;
}

void AntEntity::setDefense(float f)
  {
    assert(f>0);
    mDefense=f;
  }
float AntEntity::getDefense() const
{
  return mDefense;
}


void AntEntity::setProvide(const AGString &pName,bool flag)
  {
    if(flag)
      mProvides.insert(pName);
    else
      mProvides.erase(pName);
  }

bool AntEntity::provides(const AGString &pName) const
{
  return(mProvides.find(pName)!=mProvides.end());
}

bool AntEntity::canFight() const
{
  return (getEnergy()>0.0 && getMorale()>=0.1 && !mDefeated);
}

bool AntEntity::isFighting() const
{
  if(mJob)
    {
      FightJob *f=dynamic_cast<FightJob*>(mJob);
      return f;
    }
  return false;
}

void AntEntity::sigJobFinished()
  {
    mJobFinished.push_back(mJob);
    mJob=0;
    eventJobFinished();
  }

void AntEntity::eventHaveDefeated(AntEntity *e)
  {
  }



float AntEntity::getFood() const
{
  return mFood;
}

void AntEntity::incFood(float v)
  {
    mFood+=v;
    mFood=std::min(1.0f,mFood);
  }

void AntEntity::starve(float pTime)
  {
    mFood-=pTime*mHunger;
    if(isStarving())
      {
        // get time for which mFood==0
        float t=-mFood/mHunger;

        // hit energy
        decEnergy(mHungerHitEnergy*t);

        // hit morale
        decMorale(mHungerHitMorale*t);


        mFood=0;
      }
  }

void AntEntity::eventMoraleLow()
  {
  }

bool AntEntity::isStarving() const
{
  return mFood<=0;
}

void AntEntity::eventStartFighting()
  {
  }
void AntEntity::eventStartMoving()
  {
  }

void AntEntity::setHunger(float f)
  {
    mHunger=f;
  }

bool AntEntity::eventHitWaterMark(bool fromAbove)
  {
    return true;
  }

void AntEntity::setOnGround(bool p)
  {
    onGround=false;
  }

void AntEntity::setOnWater(bool p)
  {
    if(p)
      {
        onWater=true;
        onGround=false;
      }
    else
      {
        onWater=false;
        onGround=true;
      }

  }
bool AntEntity::isMoving() const
{
  if(!mJob)
    return false;
  return dynamic_cast<MoveJob*>(mJob);
}

AGVector2 AntEntity::getTargetPos2D()
  {
    if(mJob) {
      MoveJob *m=dynamic_cast<MoveJob*>(mJob);
      if(m)
        return m->getTargetPos2D();
    }
    return getPos2D();
  }


AntEntity *AntEntity::getTarget()
  {
    if(mJob)
      {
        FightJob *f=dynamic_cast<FightJob*>(mJob);
        if(f)
          return f->getTarget();

        MoveJob *m=dynamic_cast<MoveJob*>(mJob);
        if(m)
          return m->getTarget();

      }
    return 0;
  }

void AntEntity::setMinimapSize(float f)
  {
    mMinimapSize=f;
  }

float AntEntity::getMinimapSize() const
{
  return mMinimapSize;
}

bool AntEntity::isOnWater() const
{
  return onWater;
}

bool AntEntity::isOnGround() const
{
  return onGround;
}

void AntEntity::experienceFull()
  {
  }

void AntEntity::incExperience(float a)
  {
    experience+=a;
    if(experience>1)
      experienceFull();
  }

bool AntEntity::defeated() const
{
  return mDefeated;
}


AntMap *AntEntity::getMap()
  {
    return mMap;
  }

  
Scene *AntEntity::getScene() {
  return dynamic_cast<Scene*>(mMap->getScene());
}

void AntEntity::playSound(const AGString& name, float minDiff)
{
  /*
    float d=((getScene()->getCamera()->getPos2D-getPos2D()).length()-INNER_VOL_SIZE;
    float vol=1;
    if(d>0) {
      vol=std:max((OUTER_VOL_SIZE-d)/OUTER_VOL_SIZE,0);
    }
    AntSound.playSoundGlobal(name,vol,minDiff)
    */
  cdebug("NOT IMPLEMENTED");
}

