#ifndef AG_MUTEX
#define AG_MUTEX

#include <rk_base.h>
#include <SDL.h>
#include <SDL_thread.h>



/**
 *
 * Simple Mutex class
 * FIXME: use C++0x standard mutexes
 */
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
