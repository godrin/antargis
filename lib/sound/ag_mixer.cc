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

#include "ag_mixer.h"
#include "ag_kill.h"
#include "rk_debug.h"
#include "ag_mutex.h"
#include "ag_fs.h"
#include "ag_config.h"
#include "ag_main.h"
#include "ag_vdebug.h"

#define USE_RWOPS

#include <SDL_mixer.h>
#include <map>
#include <set>

Mix_Music *mMusic=0;
bool mMusicFinished=false;
bool mMusicInited=false;

bool mNoSound=false;

const int cSoundChannels=16;

std::map<std::string,Mix_Chunk*> mSounds;

std::set<int> mFreeChannels;
AGMutex *mSoundMutex;

void musicFinished()
{
  mMusicFinished=true;
}
void channelDone(int channel)
{
  mSoundMutex->enter();
  mFreeChannels.insert(channel);
  mSoundMutex->leave();
}
int getFreeChannel()
{
  mSoundMutex->enter();
  int c=-1;
  if(mFreeChannels.size())
  {
    c=*mFreeChannels.begin();
    mFreeChannels.erase(c);
  }
  mSoundMutex->leave();
  return c;
}


class AGPrivateSoundNotifier:public AGRepeatedCall
{
  public:
    void call()
    {
      getSoundManager()->checkFinished();
    }
};

static AGPrivateSoundNotifier *privateSoundNotifier=0;


void initSoundEngine()
{
  if(!mMusicInited)
  {
    mSoundMutex=new AGMutex;

    size_t chunkSize=1024;
    if(getConfig()->get("mixerChunkSize")=="2048")
      chunkSize=2048;
    if(getConfig()->get("mixerChunkSize")=="4096")
      chunkSize=4096;
    if(getConfig()->get("mixerChunkSize")=="8192")
      chunkSize=8192;

    assertGL;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, chunkSize)!=0) {
      printf("Mix_OpenAudio: %s\n", Mix_GetError());
      printf("Disabling sound\n");
      mNoSound=true;
      return;
    }
    assertGL;
    mMusic=0;
    Mix_HookMusicFinished(musicFinished);
    assertGL;

    // enable mixing

    Mix_AllocateChannels(cSoundChannels);
    assertGL;
    for(int i=0;i<cSoundChannels;i++)
      mFreeChannels.insert(i);

    Mix_ChannelFinished(channelDone);
    assertGL;
    mMusicInited=true;
  }

  if(!privateSoundNotifier)
  {
    privateSoundNotifier=new AGPrivateSoundNotifier;
  }
}

void closeSoundEngine()
{
  if(mNoSound)
    return;
  if(mMusicInited)
  {
    if(mMusic)
    {
      // free music
      Mix_FreeMusic(mMusic);
      mMusic=0;
    }
    //FIXME: readd this
    Mix_CloseAudio();
    checkedDelete(mSoundMutex);
  }
}


AGSound::~AGSound() throw()
{

  closeSoundEngine();
}

bool AGSound::playMp3(const std::string &pFilename)
{
  if(mNoSound)
    return false;
  if(mMusic)
  {
    return false;
  }

  std::string filename=findFile(pFilename);

  initSoundEngine();
  // load the MP3 file "music.mp3" to play as music
  mMusic=Mix_LoadMUS(filename.c_str());
  if(!mMusic) {
    printf("Mix_LoadMUS(\"%s\"): %s\n",filename.c_str(), Mix_GetError());
    //    assertGL;
    return false;
  }

  // this might be a critical error...



  // play music forever
  // Mix_Music *music; // I assume this has been loaded already
  // -1 is forever
  // 0 is never
  // 1 is once
  if(Mix_PlayMusic(mMusic, 1)==-1) {
    printf("Mix_PlayMusic: %s\n", Mix_GetError());
    //    assertGL;
    // well, there's no music, but most games don't break without music...
  }



  mMusicFinished=false;

  // set hook

  //  assertGL;

  return true;
}

bool AGSound::isMusicPlaying() const
{
  return mMusicFinished;
}

void AGSound::stopMp3()
{
  if(mNoSound)
    return;
  if(mMusic)
  {
    Mix_HaltMusic();
    Mix_FreeMusic(mMusic);
    mMusic=0;
  }
}
AGSound::AGSound():AGMessageObject(),sigMp3Finished(this,"sigMp3Finished")
{
  REGISTER_SINGLETON(this);
  soundVol=1.0f;

  mNoSound=getConfig()->get("soundEnabled")=="false";
  getConfig()->set("soundEnabled",mNoSound?"false":"true");
}

void AGSound::checkFinished()
{
  if(mNoSound)
    return;
  if(mMusicInited)
    if(mMusicFinished)
    {
      Mix_FreeMusic(mMusic);
      mMusic=0;


      sigMp3Finished(new AGEvent(this,"musicFinished"));
      mMusicFinished=false;
    }
}

void AGSound::fadeOutMusic(int ms)
{
  if(mNoSound)
    return;
  assert(ms>0);
  Mix_FadeOutMusic(ms);
}

void AGSound::volumeSound(int i,float v)
{
  int mv=((int)(v*MIX_MAX_VOLUME));
  mv=std::min(std::max(0,mv),MIX_MAX_VOLUME);
  if(i>=0 && i<cSoundChannels)
    Mix_Volume(i,mv);
}


void AGSound::volumeSound(float v)
{
  if(mNoSound)
    return;
  initSoundEngine();
  if(mNoSound)
    return;
  int mv=((int)(v*MIX_MAX_VOLUME));
  mv=std::min(std::max(0,mv),MIX_MAX_VOLUME);
  for(int i=0;i<cSoundChannels;++i)
    Mix_Volume(i,mv);
  soundVol=v;
}
void AGSound::volumeMusic(float v)
{
  if(mNoSound)
    return;
  initSoundEngine();
  Mix_VolumeMusic(((int)(v*MIX_MAX_VOLUME)));
}



void AGSound::playWave(const std::string &pFilename,float volume)
{
  if(mNoSound)
    return;
  initSoundEngine();
  assertGL;
  if(volume<0)
    volume=soundVol;
  else
    volume*=soundVol;
  if(mFreeChannels.size()>0)
  {
    cdebug("load Wave...");
    loadWave(pFilename);
    int channel=getFreeChannel();
    if(channel>=0)
    {
      Mix_Chunk *c=mSounds[pFilename];
      cdebug("Mix_Volume..");
      Mix_Volume(channel,(int)(std::min(1.0f,volume)*MIX_MAX_VOLUME));
      cdebug("PlayChannel..");
      Mix_PlayChannel(channel,c,0);
    }
  }

}


int AGSound::loopPlay(const std::string &pFilename,float volume)
{
  if(mNoSound)
    return false;
  initSoundEngine();
  if(volume<0)
    volume=soundVol;
  if(mFreeChannels.size()>0)
  {
    loadWave(pFilename);
    int channel=getFreeChannel();
    if(channel>=0)
    {
      Mix_Chunk *c=mSounds[pFilename];
      Mix_Volume(channel,(int)(std::min(1.0f,volume)*MIX_MAX_VOLUME));
      Mix_PlayChannel(channel,c,-1);
    }
    return channel;
  }
  return -1;
}
void AGSound::stopChannel(int i,int ms)
{
  if(mNoSound)
    return;
  if(i>=0 && i<cSoundChannels)
    Mix_FadeOutChannel(i,ms);
  channelDone(i);
}

void AGSound::stopAllChannels(int ms)
{
  if(mNoSound)
    return;
  for(int i=0;i<cSoundChannels;i++)
    if(mFreeChannels.find(i)==mFreeChannels.end())
      stopChannel(i,ms);
}



void AGSound::loadWave(const std::string &pFilename)
{
  if(mNoSound)
    return;
  std::map<std::string,Mix_Chunk*>::iterator i=mSounds.find(pFilename);
  if(i!=mSounds.end())
    return;

  Mix_Chunk *sample;
  std::string file=loadFile(pFilename);
  cdebug("Mix_LoadWAV_RW..");
  sample=Mix_LoadWAV_RW(SDL_RWFromMem(const_cast<char*>(file.c_str()),file.length()),1);
  cdebug("Mix_LoadWAV_RW.");

  mSounds[pFilename]=sample;
}



AGSound *mSoundManager=0;
AGSound *getSoundManager()
{
  if(!mSoundManager)
  {
    mSoundManager=new AGSound;
  }

  return mSoundManager;
}


