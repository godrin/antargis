#ifndef __RENDERER_H
#define __RENDERER_H

#include "ag_gl.h"

class Scene;
class AGFBO;

class Renderer
{
  Renderer();
 public:
  ~Renderer();

  bool canMultitexture();
  bool canShadow();
  bool canGLSL();

  GLint getShadowUnit();
  GLint getNormalUnit();

  void setCurrentScene(Scene *scene);
  Scene *getCurrentScene();

  void initShadowTexture();
  void beginShadowComputation();
  void endShadowComputation();

  void beginShadowDrawing();
  void endShadowDrawing();

  bool badShadowMap();
  
 private:

  
  int mCanMultitexture;
  int mCanShadow;
  int mCanGLSL;

  GLuint shadowMapTexture;

  AGFBO *mFBO;

  int shadowMapSize;
  bool shadowInited;

  Scene *mScene;

  friend Renderer *getRenderer();
};

Renderer *getRenderer();

#endif

