#include "ant_game_app.h"
#include "map.h"
#include "ag_layout.h"
#include "ant_hero.h"
#include "ant_human_player.h"

AntGameApp::AntGameApp ( int w, int h ) : GLApp ( w, h ) {
    controlsDisabled=false;
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

void AntGameApp::eventEntitiesClicked(PickResult pNodes, int button)
{
  AntEntity *entity=0;
  
 resetJob();
    // find first entity that's nearest to the camera
    selectEntity(0);
    for(PickResult::iterator pickIterator=pNodes.begin();pickIterator!=pNodes.end();pickIterator++) {
      SceneNode *sceneNode=pickIterator->node;
      if(dynamic_cast<Mesh*>(sceneNode) || dynamic_cast<AnimMesh*>(sceneNode)) {
        entity=getMap()->getEntity(sceneNode);
        if(entity) {
          selectEntity(entity);
          break;
        }
        
      }
    }

    if(button==1) {
      // left button == select
    } else if(button==3 && entity) {
      // right button == fight or goto
      AntBoss *boss=dynamic_cast<AntBoss*>(entity);
        if(boss) {
          if(boss->getPlayer()!=getMap()->getMyPlayer()) {
            getCurrentHero()->newHLFightJob(boss);
            return
          }
        elsif @target.is_a?(AntAnimal)
          @hero.newHLFightAnimalJob(@target)
          return
        end
        # move near target
        @hero.newHLMoveJob(0,@target.getPos2D,2)
      end
    }

}


