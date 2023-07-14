#ifndef __3D_ENGINE_H
#define __3D_ENGINE_H

#include "ant_renderer.h"
class Ant3dEngine {
  public:

    Ant3dEngine();
    ~Ant3dEngine();

    Renderer *getRenderer();
  
  private:
    Renderer *mRenderer;

};


#endif
