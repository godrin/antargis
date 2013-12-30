/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_sound.h
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

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef AG_MIXER_H
#define AG_MIXER_H

#include <string>
#include "ag_messageobject.h"

class AGEXPORT AGSound:public AGMessageObject
{
 public:
  AGSound(); // NEVER use this! - it's only provided due to swig
  ~AGSound() throw();
  bool playMp3(const std::string &pFilename);

  bool isMusicPlaying() const;

  void stopMp3();
  void checkFinished();
  void fadeOutMusic(int ms);

  void playWave(const std::string &pFilename,float v=-1.0);
  int loopPlay(const std::string &pFilename,float v=-1.0);
  void stopChannel(int i,int ms=200);
  void stopAllChannels(int ms=200);

  void volumeSound(int i,float v); // 0 to 1
  void volumeSound(float v); // 0 to 1
  void volumeMusic(float v); // 0 to 1

  void loadWave(const std::string &pFilename);


  AGSignal sigMp3Finished;
 private:

  friend AGSound *getSoundManager();
  float soundVol;

};

AGEXPORT AGSound *getSoundManager();
AGEXPORT void initSoundEngine();

#endif

