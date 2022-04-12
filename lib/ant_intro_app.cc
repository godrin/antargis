#include "ant_intro_app.h"
#include "ag_image.h"
#include "ag_screen.h"
#include "ant_black_fade_widget.h"
#include <ag_color.h>
#include <ag_fontengine.h>
#include <ag_local.h>
#include <ag_mixer.h>
#include <ag_theme.h>
#include <rk_debug.h>

AntIntroApp::AntIntroApp() {
  screens.push_back("rough lands");
  screens.push_back("strong heroes");
  screens.push_back("data/gui/logoGold.png");
  screens.push_back("your mission");
  screens.push_back("unify the people");
  screens.push_back("data/gui/castle_small.png");
}
AntIntroApp::~AntIntroApp() throw() {}
void AntIntroApp::init() {
  AGFont font = getTheme()->getFont("intro.font");

  for (std::vector<std::string>::iterator i = screens.begin();
       i != screens.end(); i++) {
    std::string postfix = i->substr(i->length() - 3, 3);
    if (postfix == "png") {
      surfaces.push_back(new AGTexture(AGSurface::load(*i)));
    } else {
      surfaces.push_back(getFontEngine()->renderText(0, 0, _(*i), font));
    }
  }

  img = new AGImage(0, getScreen().getRect(), *surfaces[0], false);
  black = new AntBlackFadeWidget(
      img, AGRect2(0, 0, getScreen().getWidth(), getScreen().getHeight()));
  img->addChild(black);

  setMainWidget(img);

  getSoundManager()->playMp3("data/music/battle1.ogg");

  phases.clear();
  phases.push_back(2);
  phases.push_back(2);
  phases.push_back(2);
  phases.push_back(1);
  phase = 0;
  time = 0;
  blendin = 0.9;
  blendout = 0.6;
  show = 1.2;
  hide = 0.3;

  s = 0;
  cancelled = false;
}

bool AntIntroApp::eventKeyDown(AGEvent *m2) {
  tryQuit();
  return AGApplication::eventKeyDown(m2);
}

void AntIntroApp::checkPhase() {
  /*
  # this function does:
  # * increase the phase - set alpha for BlackFade-Widget
  # * switch to newer textures/text-elements
  # * quit the intro, when its finished
  */
  if (time > phases[phase]) {

    time -= phases[phase];
    phase += 1;
    if (phase == 4)
      phase = 0;
    else if (phase ==
             3) { // # switch surface early, so there won't be a timing problem
      s = s + 1;
      if (s == surfaces.size()) {
        s = 0;
        cancelled = true;
      }
      img->setTexture(*surfaces[s]);
    }
  }
  switch (phase) {
  case 0:
    black->setAlpha(1 - time / phases[phase]);
    break;
  case 2:
    black->setAlpha(time / phases[phase]);
    break;
  case 3:
    if (cancelled)
      tryQuit();
    break;
  }
  img->redraw();
}

bool AntIntroApp::eventFrame(float pTime) {
  time += pTime;
  checkPhase();
  SDL_Delay(10);
  return false;
}
