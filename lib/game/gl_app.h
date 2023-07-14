#ifndef ANT_APP_H
#define ANT_APP_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

/**
 *
 * \mainpage Battles of Antargis - C++ documentation
 *  \section Overview
 *    This is the C++ part of the API-documentation.
 *    It consists of three major parts:
 *    \see AntargisGUI
 *    \see Engine3d
 *    \see GameEngine
 *
 *    For information about the overall-architecture please refer to the
 * ruby-documentation at: ../ruby/index.html
 *
 */

#include "ant_renderer.h"
#include "scene.h"
#include <ag_application.h>
#include <ag_geometry.h>
#include <ag_main.h>
#include <fstream>
#include <iostream>
#include <rk_debug.h>

#ifndef SWIG
extern std::ofstream out;
#endif

class AGEXPORT GLApp : public AGApplication {
  bool shadow;
  float mx, my;
  float omx, omy;
  AGVector4 camera;
  float frameTime;
  size_t frameCount;

  bool mMayClick;

  float hx, hy;
  int hb;

protected:
  Scene *scene;
  Renderer *renderer;

public:
  GLApp(int w, int h);
  ~GLApp() throw();

  virtual void draw();
  void drawGL();

  bool eventFrame(float t);

  virtual void eventClick(const PickResult &pNodes, int button);
  virtual void eventHover(const PickResult &pNodes, int button);

  virtual bool eventMouseButtonDown(AGEvent *m);
  virtual bool eventMouseButtonUp(AGEvent *m);
  virtual bool eventMouseMotion(AGEvent *m);
  virtual bool eventKeyDown(AGEvent *m);
  virtual void eventCameraMoved();

  Scene &getScene();

  virtual float getCameraHeight(const AGVector2 &p);
  virtual void setCamera(const AGVector2 &p);
  AGVector2 getCamera() const;
};

#endif
