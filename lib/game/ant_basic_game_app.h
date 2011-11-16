#ifndef __BASIC_GAME_APP_H
#define __BASIC_GAME_APP_H


#include "gl_app.h"

class AntBasicGameApp:public GLApp {
public:
    AntBasicGameApp(int w, int h);

    virtual void eventClick(const PickResult &pNodes,int button);
private:
  bool controlsDisabled;
};

#endif
