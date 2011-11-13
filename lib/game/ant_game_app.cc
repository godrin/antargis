#include "ant_game_app.h"
#include "map.h"
#include "ag_layout.h"
#include "ant_hero.h"
#include "ant_human_player.h"

AntGameApp::AntGameApp ( int w, int h ) : GLApp ( w, h ) {
}

void AntGameApp::init() {
    layout=new AGLayout ( 0 );
    layout->loadXML ( loadFile ( "data/gui/layout/ant_layout.xml" ) );
    layout->setApp ( this );
    setMainWidget ( layout );

    mMap=new AntMap ( &getScene(),32,32 );
    mMap->loadMap("data/levels/tutorial/tutorial3.antlvl");
//    mMap->loadMap ( "data/levels/birth/birth1.antlvl" );

    // FOCUS hero
    setCamera(getPlayerHero()->getPos2D());
}

bool AntGameApp::eventFrame ( float pTime ) {
    bool r=GLApp::eventFrame ( pTime );

    mMap->move ( pTime );
    getScene().advance(pTime);
    SDL_Delay(20);

    return r;
}

AntHero* AntGameApp::getPlayerHero() {
    AntPlayer *p=mMap->getMyPlayer();
    cdebug("PLAYER:"<<p);
    cdebug("heroes:"<<p->getHeroes().size());
    AntHero *h=p->getHeroes().front();
    cdebug("HERO:"<<h<<" "<<h->getName());
    
return h;
}
