#include "entities.h"
#include "ant_hero.h"
#include "ant_tree.h"
#include "ant_sheep.h"

AntEntity *createEntity ( const Node &node,AntMap *map ) {

    AntEntity *e=0;
    if ( node.getName() =="antHero" ) {
        e=new AntHero ( map );
    } else if ( node.getName() =="antNewTree" ) {
        e=new AntTree ( map );
    } else if ( node.getName() =="antNewSheep" ) {
        e=new AntSheep ( map );
    }
    
    

    if ( e ) {
        e->init();
        e->loadXML(node);
    }
    return e;
}
