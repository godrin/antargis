#ifndef __BASIC_GAME_APP_H
#define __BASIC_GAME_APP_H

#include "gl_app.h"
class AntEntity;
class AntMap;

class AntBasicGameApp:public GLApp {
public:
    AntBasicGameApp(int w, int h);

    virtual void eventClick(const PickResult &pNodes,int button);
    virtual void eventHover(const PickResult &pNodes,int button);
protected:
    virtual void eventMapClicked(const AGVector2 &mapPos, int button)=0;
    virtual void eventEntitiesClicked(const PickResult &pNodes, int button)=0;
    virtual AntMap *getMap()=0;
    virtual void hoverEntity(AntEntity *e);
private:
    bool controlsDisabled;
    int hoveredEntityId;
};

#endif
