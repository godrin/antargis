#include "ag_mutex.h"
#include <stdlib.h>

AGMutex::AGMutex()
{
  mut=SDL_CreateMutex();

}
AGMutex::~AGMutex()
{
  SDL_DestroyMutex(mut);
}

void AGMutex::enter()
{
  if(SDL_mutexP(mut)==-1){
    fprintf(stderr, "Couldn't lock mutex\n");
    exit(-1);
  }
}

void AGMutex::leave()
{
  if(SDL_mutexV(mut)==-1){
    fprintf(stderr, "Couldn't unlock mutex\n");
    exit(-1);
  }
}
