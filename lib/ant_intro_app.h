#ifndef __ANT_INTRO_APP_h
#define __ANT_INTRO_APP_h

#include <ag_application.h>
#include <ag_surface.h>

#include <string>
#include <vector>

class AGImage;
class AntBlackFadeWidget;

class AntIntroApp : public AGApplication {
public:
  AntIntroApp();
  virtual ~AntIntroApp() throw();
  void init();
  bool eventKeyDown(AGEvent *m2);
  bool eventFrame(float pTime);

private:
  void checkPhase();

  std::vector<std::string> screens;
  std::vector<AGTexture *> surfaces;
  std::vector<size_t> phases;

  AGImage *img;
  AntBlackFadeWidget *black;
  size_t phase;
  float time;
  float blendin, blendout;
  float show, hide;
  float s;
  bool cancelled;
};

#endif
