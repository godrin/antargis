/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_main.cc
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

#include <iostream>

#include "rk_base.h"
#include "rk_debug.h"
#include "rk_tools.h"

#include "ag_main.h"
#include "ag_fs.h"

#include <ruby.h>

int lastWidth=0;
int lastHeight=0;
int lastVWidth=0;
int lastVHeight=0;
int lastDepth=0;
bool fullScreen=false;
bool lastGL=false;

AGMain *mAGMain=0;

bool quited=false;

// singleton-management
void newInstanceKiller();
void deleteInstanceKiller();

#ifdef DRMUSER
std::string gUserName=DRMUSER;
bool gDRMok=false;
#endif

//FIXME: discard this function!
bool hasQuit()
  {
    return quited;
  }
/*
bool glMode()
{
  return lastGL;
}*/


/**
   creates an AGMain object.
 */
AGMain::AGMain():
  mCollector(0),mRand(0)
  {
    assert(mAGMain==0);
    mAGMain=this;

    mRand=0;

    newInstanceKiller();

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE|SDL_INIT_AUDIO)<0)
      {
        cdebug("SDL could not be initialized!");
        exit(1);
      }
    SDL_EnableUNICODE(1);

    videoInfo=0;

    initFS("");

    mVideo=0;
  }

AGMain::~AGMain() throw()
  {
    CTRACE;
    saveDelete(mVideo);
    deleteInstanceKiller();
    mAGMain=0;
    cdebug("QUIT");
    SDL_Quit();
    quited=true;
    setQuitting();

    //  saveDelete(mRand);


    setRubyRaising(true);
  }


AGCollector *AGMain::getCollector()
  {
    if(!mCollector)
      mCollector=new AGCollector;
    return mCollector;
  }



bool hasMain()
  {
    return mAGMain;
  }

AGMain *getMain()
  {
    if(!mAGMain)
      {
        std::cerr<<"AGMain not initialized!"<<std::endl;
        assert(0==1);
        throw std::runtime_error("AGMain not initialized");
        exit(1);
      }


    static bool registered=false;
    if(!registered)
      {
        // registered must be set here, so that we don't get into an endless recursion
        registered=true;

        // IMPORTANT:
        //   put it into a global variable - so that it won't get garbage collected
        rb_eval_string("include Antargis");
        //rb_eval_string("Antargis");
        registered=(rb_eval_string("if Antargis.respond_to?(:getMain) \n $agMain=getMain\n true \n else \n false \nend")==Qtrue);
        cdebug("registered:"<<registered);
      }


    return mAGMain;
  }

long AGMain::getTicks() const
{
  return SDL_GetTicks();
}


void AGMain::mark() throw()
  {
    markObject(getCollector());
    markObject(mRand);
  }

void AGMain::setRand(AGRandomizerBase *pRand)
  {
    mRand=pRand;
  }

AGRandomizerBase *AGMain::getRand()
  {
    return mRand;
  }

AGVideoBase *AGMain::getVideo()
  {
    return mVideo;
  }


bool videoInited()
  {
    return getMain()->getVideo();
  }


std::string gAppName="Antargis";
void setAppName(const std::string &pName)
  {
    gAppName=pName;
  }
std::string getAppName()
  {
    return gAppName;
  }


void AGMain::repeatedCalls()
  {
    for(std::set<AGRepeatedCall*>::iterator i=mCalls.begin();i!=mCalls.end();i++)
      {
        (*i)->call();
      }
  }

void AGMain::setVideo(AGVideoBase *p)
  {
    mVideo=p;
  }

void AGMain::delay(int ms)
  {
    SDL_Delay(ms);
  }




///   AGRepeatedCall
AGRepeatedCall::AGRepeatedCall()
  {
    assert(hasMain());
    getMain()->mCalls.insert(this);
  }
AGRepeatedCall::~AGRepeatedCall()
  {
    if(hasMain())
      getMain()->mCalls.erase(this);
  }
void AGRepeatedCall::call()
  {
  }
