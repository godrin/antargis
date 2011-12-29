#ifndef __ANT_GAME_APP_H
#define __ANT_GAME_APP_H

#include "ant_basic_game_app.h"

class AntMap;
class AntHero;
class AntEntity;

class AntGameApp:public AntBasicGameApp {
public:
    AntGameApp ( int w, int h );
    void init(const std::string &level);

    virtual bool eventFrame ( float pTime );


    AntHero *getCurrentHero();
protected:
    void eventMapClicked(const AGVector2 &pos, int button);
    void eventEntitiesClicked(const PickResult &pNodes, int button);
    void selectEntity(AntEntity *e);
    AntMap *getMap();
private:

    void resetJob();
    
    AntEntity *entity;
    AntMap *mMap;
    AGLayout *layout;
    AntHero *currentHero;

};

#endif
