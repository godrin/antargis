#ifndef AG_MUTEX
#define AG_MUTEX

#include <rk_base.h>
#include <SDL.h>
#include <SDL_thread.h>

class AGEXPORT AGMutex
{
 public:
  AGMutex();
  ~AGMutex();

  void enter();
  void leave();
 private:
  SDL_mutex *mut;

};

#endif
