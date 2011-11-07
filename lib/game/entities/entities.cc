#include "entities.h"
#include "ant_hero.h"

AntEntity *createEntity ( const Node &node,AntMap *map ) {

    AntEntity *e=0;
    if ( node.getName() =="antHero" ) {
        e=new AntHero ( map );
    }

    if ( e )
        e->init();
    return e;
}
