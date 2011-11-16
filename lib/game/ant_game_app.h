#ifndef __ANT_GAME_APP_H
#define __ANT_GAME_APP_H

#include "ant_basic_game_app.h"

class AntMap;
class AntHero;

class AntGameApp:public AntBasicGameApp {
public:
    AntGameApp ( int w, int h );
    void init();

    virtual bool eventFrame ( float pTime );

    AntHero *getPlayerHero();


protected:
    void eventMapClicked(AGVector4 pos, int button);
    void eventEntitiesClicked(PickResult pNodes, int button);
private:
    AntMap *mMap;
    AGLayout *layout;
};

#endif
