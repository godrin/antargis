#include "ant_game_app.h"
#include "map.h"
#include "ag_layout.h"
#include "ant_hero.h"
#include "ant_human_player.h"
#include "ant_boss.h"
#include "ant_animal.h"
#include "mesh.h"
#include "anim_mesh.h"
#include "ant_hljob_fighting.h"
#include "ant_hljob_fight_animal.h"
#include "ant_path_finder_complete.h"

#include "ant_action_widget.h"

AntGameApp::AntGameApp ( int w, int h ) : AntBasicGameApp ( w, h )
{
}

void AntGameApp::init(const std::string &level)
{
    layout=new AGLayout ( 0 );
    layout->loadXML ( loadFile ( "data/gui/layout/ant_layout.xml" ) );
    layout->setApp ( this );
    setMainWidget ( layout );

    mMap=new AntMap ( &getScene(),32,32 );
    mMap->loadMap (level);// "data/levels/tutorial/tutorial3.antlvl" );
//    mMap->loadMap ( "data/levels/birth/birth1.antlvl" );

    // init path finder
    AntPathFinderComplete *pf=new AntPathFinderComplete ( mMap );

    mMap->setCompletePathFinder ( pf );
    currentHero=mMap->getMyPlayer()->getHeroes().front();
    // FOCUS hero
    setCamera ( currentHero->getPos2D() );
}

bool AntGameApp::eventFrame ( float pTime )
{
    bool r=GLApp::eventFrame ( pTime );

    mMap->move ( pTime );
    getScene().advance ( pTime );
    SDL_Delay ( 20 );

    return r;
}

void AntGameApp::eventEntitiesClicked ( const PickResult &pNodes, int button )
{
    CTRACE;
    AntEntity *entity=0;

    resetJob();
    // find first entity that's nearest to the camera
    selectEntity ( 0 );
    for ( PickResult::const_iterator pickIterator=pNodes.begin();pickIterator!=pNodes.end();pickIterator++ )
    {
        SceneNode *sceneNode=pickIterator->node;
        if ( dynamic_cast<Mesh*> ( sceneNode ) || dynamic_cast<AnimMesh*> ( sceneNode ) )
        {
            entity=mMap->getEntity ( sceneNode );
            if ( entity )
            {
                selectEntity ( entity );
                break;
            }
        }
    }

    if ( button==1 )
    {
        // left button == select
        
        AntActionWidget *w=new AntActionWidget(getMainWidget(),AGRect2(50,0,140,40));
	std::vector<AntActionWidget::Action> actions;
	actions.push_back(AntActionWidget::FIGHT);
	actions.push_back(AntActionWidget::HARVEST);
	AGVector2 pos(100,90);
	w->show(actions,pos);
	//getMainWidget()->addChild(w);
	getMainWidget()->addChild(w);
    }
    else if ( button==3 && entity )
    {
        // right button == fight or goto
        AntBoss *boss=dynamic_cast<AntBoss*> ( entity );
        AntAnimal *animal=dynamic_cast<AntAnimal*> ( entity );
        if ( boss )
        {
            if ( boss->getPlayer() !=mMap->getMyPlayer() )
            {
                getCurrentHero()->setHlJob ( new AntHlJobFighting ( getCurrentHero(),boss ) );
                return;
            }
        }
        else if ( animal )
        {
            //FIXME
            //getCurrentHero()->setHlJob(new AntHLJobFightAnimal(animal));
            return;
        }
        // move near target
        getCurrentHero()->setHlJob ( new AntHLJobMoving ( getCurrentHero(),entity->getPos2D(),2 ) );
    }
}


void AntGameApp::selectEntity ( AntEntity* e )
{
    if ( entity )
        entity->selected ( false );
    entity=e;
    if ( entity )
        entity->selected ( true );
}

AntHero* AntGameApp::getCurrentHero()
{
    return currentHero;
}

void AntGameApp::resetJob()
{
//FIXME
}

void AntGameApp::eventMapClicked ( const AGVector2 &pos, int button )
{
    CTRACE;
    
    selectEntity(0);
    //FIXME
    /*
      if @job and button==1 then
        case @job
          when "doBuild"
            buildHouse(pos.dim2)
            @job=nil
        end
        resetJob
        return
      end
      */
    if ( getCurrentHero() && button==3 )
    {
// assign hero a move job
        getCurrentHero()->setHlJob ( new AntHLJobMoving ( getCurrentHero(),pos,0 ) );

    }
}

AntMap* AntGameApp::getMap()
{
    return mMap;
}
