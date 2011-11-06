#include "entities.h"
#include "ant_hero.h"

AntEntity *createEntity ( const Node &node,AntMap *map ) {

    if ( node.getName() =="antHero" ) {
        return new AntHero(map);
    }
    return 0;
}
