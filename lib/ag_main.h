/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_main.h
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

#ifndef __AG_MAIN
#define __AG_MAIN

#include <ag_rand_base.h>
#include <ag_video_base.h>

#include <string>
#include <set>

/**
 * \defgroup AntargisGUI AntargisGUI
 * \brief the GUI-library of the game Battles of Antargis
 *
 * This library consists of different mechanisms to make your life easier
 * with graphical user interfaces:
 * \see widgets
 * \see application
 * \see events
*/

class AGScreen;

struct SDL_VideoInfo;

// will be called repeatedely (by AGApplication - if available)
// simply instantiate a subclass of this type - and it'll be registered within AGMain
class AGEXPORT AGRepeatedCall
{
 public:
  AGRepeatedCall();
  virtual ~AGRepeatedCall();
  virtual void call();
};

class AGEXPORT AGMain
{
 public:
  AGMain();
  ~AGMain() throw ();

  long getTicks() const;

  void setRand(AGRandomizerBase *pRand);
  AGRandomizerBase *getRand();


  AGVideoBase *getVideo();
  void setVideo(AGVideoBase *p);

  void repeatedCalls();

  void delay(int ms);

 protected:

 private:
  AGVideoBase *mVideo;

  const SDL_VideoInfo *videoInfo;

  AGRandomizerBase *mRand;

  std::set<AGRepeatedCall*> mCalls;
  friend class AGRepeatedCall;
};

AGEXPORT bool hasMain();
AGEXPORT AGMain *getMain();

AGEXPORT bool hasQuit();

AGEXPORT void setAppName(const std::string &pName);
AGEXPORT std::string getAppName();
//std::string AGEXPORT myHash(const std::string &p);

// from ag_debug
AGEXPORT size_t getDebugLevel();
AGEXPORT void setDebugLevel(size_t t);
AGEXPORT void setRubyRaising(bool flag);

AGEXPORT bool videoInited();


#endif
