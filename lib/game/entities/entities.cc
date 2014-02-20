#include "entities.h"
#include "ant_hero.h"
#include "ant_tree.h"
#include "ant_sheep.h"
#include "ant_tower.h"
#include "ant_grass.h"
#include "ant_bush.h"
#include "ant_man.h"
#include "ant_fire.h"
#include "ant_workshop.h"
#include "ant_farm.h"
#include "ant_sack.h"
#include "ant_stone.h"
#include "ant_decal.h"

AntEntity *createEntity ( const Node &node,AntMap *map ) {

    AntEntity *e=0;
    if ( node.getName() =="antHero" ) {
        e=new AntHero ( map );
    } else if ( node.getName() =="antMan" ) {
        e=new AntMan ( map );
    } else if ( node.getName() =="antTree" || node.getName() =="antNewTree" ) {
        e=new AntTree ( map );
    } else if ( node.getName() =="antNewSheep" ) {
        e=new AntSheep ( map );
    } else if ( node.getName() =="antNewStone" ) {
        e=new AntStone ( map );
    } else if ( node.getName() =="antGrass" ) {
        e=new AntGrass ( map );
    } else if ( node.getName() =="antHighGrass" ) {
        e=new AntGrass ( map,true );
    } else if ( node.getName() =="antSack" ) {
        e=new AntSack ( map );
    } else if ( node.getName() =="antBush" ) {
        e=new AntBush ( map );
    } else if ( node.getName() =="antTower" ) {
        e=new AntTower ( map );
    } else if ( node.getName() =="antWorkshop" ) {
        e=new AntWorkshop ( map );
    } else if ( node.getName() =="antFarm" ) {
        e=new AntFarm ( map );
    } else if ( node.getName() =="antFire" ) {
        e=new AntFire ( map );
    } else if ( node.getName() =="antDecal" ) {
        e=new AntDecal ( map );
    } else if ( node.getName() =="lazyPlayer" ) {
      //FIXME
    } else if ( node.getName() =="humanPlayer" ) {
      //FIXME
    } else if ( node.getName() =="antFire" ) {
    } else if(node.getName()!=""){
      throw std::runtime_error("unknown node "+node.getName());
    }

    if ( e ) {
        e->init();
        e->loadXML(node);
        cdebug("NEW ENTITY:"<<typeid(*e).name()<<":"<<e->getName()<<":"<<e->getID());
    }
    return e;
}
