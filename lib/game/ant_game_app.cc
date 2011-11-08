#include "ant_game_app.h"
#include "map.h"
#include "ag_layout.h"
#include "ant_hero.h"

AntGameApp::AntGameApp ( int w, int h ) : GLApp ( w, h ) {
}

void AntGameApp::init() {
    layout=new AGLayout ( 0 );
    layout->loadXML ( loadFile ( "data/gui/layout/ant_layout.xml" ) );
    layout->setApp ( this );
    setMainWidget ( layout );

    mMap=new AntMap ( &getScene(),32,32 );
    mMap->loadMap("data/levels/tutorial/tutorial0.antlvl");
//    mMap->loadMap ( "data/levels/birth/birth1.antlvl" );

    // FOCUS hero
    setCamera(getPlayerHero()->getPos2D());
}

bool AntGameApp::eventFrame ( float pTime ) {
    bool r=GLApp::eventFrame ( pTime );

    mMap->move ( pTime );

    return r;
}

AntHero* AntGameApp::getPlayerHero() {
    std::list<AntEntity*> l=mMap->getAllEntities();
    for ( std::list<AntEntity*>::iterator i=l.begin();i!=l.end();i++ ) {
        AntHero *h=dynamic_cast<AntHero*> ( *i );
        if ( h && h->isPrimary() )
            return h;
    }

    return 0;
}
