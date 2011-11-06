/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * jobs.h
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

#ifndef ANT_JOBS_H
#define ANT_JOBS_H
#include <ag_geometry.h>

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

class AntEntity;

/**
   Job class won't be exported to Ruby, because their constant
   creation and deletion fills up the GC otherwise.
*/
class AGEXPORT Job
{
  int priority;
  bool inited;
 public:
  Job();
  Job(int p);
  virtual ~Job();
  virtual void move(AntEntity *,float ptime);

  virtual void saveXML(Node &pNode) const;
  virtual void loadXML(const Node &pNode);

  virtual AGString xmlName() const;

  virtual void jobFinished(AntEntity *e);
  bool operator<=(const Job &j) const;

  virtual bool needsMorale() const;

  bool valid() const;
};

class AGEXPORT RestJob:public Job
{
  float mTime;
  bool mWork;
 public:
  RestJob();
  RestJob(float pTime,bool pWork=false);
  virtual ~RestJob();
  void move(AntEntity *e,float ptime);

  virtual void saveXML(Node &pNode) const;
  virtual void loadXML(const Node &pNode);
  virtual AGString xmlName() const;
};

class AGEXPORT MoveJob:public Job
{
  AGVector2 mTarget;
  AGVector3 mTarget3;
  AGVector2 mNearTarget;
  AntEntity *mTargetEntity;
  float mNear;
  bool mRun;

  bool m3d;
 public:
  MoveJob();
  MoveJob(int p,AntEntity *pTarget,float pnear=0,bool pRun=false);
  MoveJob(int p,const AGVector2 &pTarget,float pnear=0,bool pRun=false);
  MoveJob(int p,const AGVector3 &pTarget,float pnear=0,bool pRun=false);

  virtual ~MoveJob();
  void move(AntEntity *e,float ptime);
  AGVector2 getDirection(const AntEntity *e) const;
  
  virtual void saveXML(Node &pNode) const;
  virtual void loadXML(const Node &pNode);
  virtual AGString xmlName() const;

  AntEntity *getTarget()
    {
      return mTargetEntity;
    }
  AGVector2 getTargetPos2D() const;
  
 private:
  void moveBy(AntEntity *e,float ptime,float aspeed);
};

// FIXME: implement near and far fighting (arrows)
class AGEXPORT FightJob:public Job
{
  AntEntity *mTarget;
  float fightDistance;
  bool moving;
  int mTargetID;
  
 public:
  FightJob();
  FightJob(int p,AntEntity *pTarget, float pDistance=1);
  
  virtual ~FightJob();
  void move(AntEntity *e,float ptime);
  virtual bool needsMorale() const;
  AntEntity *getTarget()
    {
      return mTarget;
    }
  
  virtual void saveXML(Node &pNode) const;
  virtual void loadXML(const Node &pNode);
  virtual AGString xmlName() const;
  
};

// FIXME: check if this can be discarded ???
class AGEXPORT FetchJob:public MoveJob
{
  AGString mWhat;
  AntEntity *mTarget;
  int mTargetID;
 public:
  FetchJob();
  FetchJob(int p,const AGVector2 &pTarget,AGString what);
  FetchJob(int p,AntEntity *pTarget,AGString what);
  virtual ~FetchJob();
  void move(AntEntity *e,float ptime);
  virtual void jobFinished(AntEntity *e);
  
  virtual void saveXML(Node &pNode) const;
  virtual void loadXML(const Node &pNode);
  virtual AGString xmlName() const;
};

#endif
