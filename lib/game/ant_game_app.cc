#include "ag_layout.h"
#include "ag_button.h"
#include "anim_mesh.h"
#include "ant_action_widget.h"
#include "ant_animal.h"
#include "ant_boss.h"
#include "ant_game_app.h"
#include "ant_hero.h"
#include "ant_hero_faces.h"
#include "ant_hljob_dismiss.h"
#include "ant_hljob_drop.h"
#include "ant_hljob_fetching.h"
#include "ant_hljob_fight_animal.h"
#include "ant_hljob_pickup_from.h"
#include "ant_hljob_fighting.h"
#include "ant_hljob_recruit.h"
#include "ant_house.h"
#include "ant_human_player.h"
#include "ant_layout_creators.h"
#include "ant_path_finder_complete.h"
#include "ant_sack.h"
#include "map.h"
#include "mesh.h"

#include <boost/bind.hpp>

std::vector<AntActionWidget::Action> getActions ( AntHero *hero,AntEntity *selectedEntity );

void testing();

AntGameApp::AntGameApp ( int w, int h ) : AntBasicGameApp ( w, h )
{
  // REVIEWED
  mMap=0;
  layout=0;
  currentHeroId=-1;
  actionWidget=0;
  mPaused=false;
  testing();
  speed=1;
}

void AntGameApp::init ( const std::string &level )
{
  initAntargisLayoutCreators();
  actionWidget=0;
  layout=new AGLayout ( 0 );
  layout->loadXML ( loadFile ( "data/gui/layout/ant_layout.xml" ) );
  layout->setApp ( this );
  setMainWidget ( layout );

  mMap=new AntMap ( &getScene(),32,32 );
  mMap->loadMap ( level );
  // init path finder
  auto myPlayer=mMap->getMyPlayer();
  AntHero *currentHero=myPlayer->getHeroes().front();
  currentHeroId=currentHero->getID();
  // FOCUS hero
  setCamera ( currentHero->getPos2D() );

  AntHeroFaces *heroFaces=dynamic_cast<AntHeroFaces*>(layout->getChild("ant_hero_faces"));
  myPlayer->sigHeroesChanged.connect([heroFaces](AntPlayer *player) { heroFaces->update(player);});
  myPlayer->sigHeroesChanged(myPlayer);
  heroFaces->sigHeroClicked.connect([this](AntHero *hero){
      std::cout<<"Hero clicked:"<<hero<<std::endl;
      this->setCamera(hero->getPos2D());
      });


  // QUIT on door click
  layout->getChild("quit")->sigClickBoost.connect([this](AGWidget *caller) {
      this->tryQuit();
      return true;
      });
  // toggle pause
  AGButton *pauseButton=dynamic_cast<AGButton*>(layout->getChild("pause"));
  pauseButton->sigClickBoost.connect([this,pauseButton](AGWidget *caller) {
      this->mPaused=!this->mPaused;
      pauseButton->setSurface(AGSurface::load(AGString("data/gui/")+ (this->mPaused?"play":"pause")+ ".png"),false);
      return true;
      });
}

bool AntGameApp::eventFrame ( float pTime )
{
  bool r=GLApp::eventFrame ( pTime );

  if(!mPaused)
    mMap->move ( pTime*speed );
  getScene().advance ( pTime*speed );
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
  for ( PickResult::const_iterator pickIterator=pNodes.begin(); pickIterator!=pNodes.end(); pickIterator++ )
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
    showActionWidget();

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

void AntGameApp::showActionWidget() {
  CTRACE;
  if ( actionWidget==0 )
  {
    actionWidget=new AntActionWidget ( getMainWidget(),AGRect2 ( 50,200,140,40 ) );
    actionWidget->setHandler ( this );
    getMainWidget()->addChildBack ( actionWidget );
  }

  std::vector<AntActionWidget::Action> actions=getActions ( getCurrentHero(),getSelectedEntity() );
  AGVector4 mpos=getMap()->getVertex(targetPos.getX(),targetPos.getY());
  AGVector2 pos;
  auto selectedEntity=getSelectedEntity();
  if(selectedEntity)
    mpos=AGVector4(selectedEntity->getPos3D(),1);
  
  pos=getScene().getPosition(mpos)+AGVector2(-20,-20);
  cdebug("POSSS:"<<pos);
  //AGVector2 pos ( 100,190 );
  actionWidget->show ( actions,pos );
  //getMainWidget()->addChild(w);
}

void AntGameApp::selectEntity ( AntEntity* e )
{
  CTRACE;
  cdebug("selectEntity:"<<e);
  AntEntity *entity=getMap()->getEntity ( selectedEntityId );
  if ( entity )
    entity->selected ( false );

  if ( e )
  {
    e->selected ( true );

    printOutEntityInfo(e);

    selectedEntityId=e->getID();
  }
  else
    selectedEntityId=-1;
}


void AntGameApp::printOutEntityInfo(AntEntity *e) {
  std::cout
    <<"=============================="<<std::endl
    <<"Type   :"<<typeid(*e).name()<<std::endl
    <<"------------------------------"<<std::endl
    <<"health :"<<e->getEnergy()<<std::endl
    <<"food   :"<<e->getFood()<<std::endl
    <<"morale :"<<e->getMorale()<<std::endl
    <<"aggres :"<<e->getAggression()<<std::endl
    ;



  AntBoss *boss=dynamic_cast<AntBoss*>(e);
  if(boss) {
    std::cout<<"men    :"<<boss->getMenWithoutBoss().size()<<std::endl;
  }
  AntMan *man=dynamic_cast<AntMan*>(e);
  if(man) {
    AntBoss *myBoss=man->getBoss();
    if(myBoss) {
      std::cout<<"boss   :"<<myBoss->getEntity()->getName()<<std::endl;
      std::cout<<"bossjob:"<<typeid(*myBoss->getHlJob()).name()<<std::endl;
    }
  }
  std::cout<<"job    :"<<e->getJobName()<<std::endl;


  std::cout<<"------------------------------"<<std::endl;
  for(auto resourceIter:e->resource.getAll()) {
    std::cout<<resourceIter.first<<": "<<resourceIter.second<<std::endl;
  }

  std::cout
    <<"=============================="<<std::endl
    ;
}

AntHero* AntGameApp::getCurrentHero()
{
  AntEntity *e= mMap->getEntity ( currentHeroId );
  AntHero *h=dynamic_cast<AntHero*> ( e );
  return h;
}

AntEntity* AntGameApp::getSelectedEntity()
{
  return mMap->getEntity ( selectedEntityId );
}


void AntGameApp::resetJob()
{
  //FIXME
}

void AntGameApp::eventMapClicked ( const AGVector2 &pos, int button )
{
  CTRACE;
  if ( actionWidget )
    actionWidget->hide();

  selectEntity ( 0 );
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
  if ( getCurrentHero() && button==1 )
  {
    targetPos=pos;
    showActionWidget();
    // assign hero a move job
    //    getCurrentHero()->setHlJob ( new AntHLJobMoving ( getCurrentHero(),pos,0 ) );
  }
}

AntMap* AntGameApp::getMap()
{
  return mMap;
}

std::vector< AntActionWidget::Action > getActions ( AntHero *hero,AntEntity *target )
{
  std::vector<AntActionWidget::Action> actions;

  actions.push_back ( AntActionWidget::STOP );
  if ( target )
  {
    AntBoss *targetBoss=dynamic_cast<AntBoss*> ( target );
    if ( targetBoss )
    {
      bool playerEqual=hero->getPlayer() == targetBoss->getPlayer();
      if ( playerEqual )
      {
        if(hero!=targetBoss) {
          actions.push_back ( AntActionWidget::TAKE_FOOD );
          actions.push_back ( AntActionWidget::TAKE_WEAPONS );
          actions.push_back ( AntActionWidget::RECRUIT );
        }
        if(hero==targetBoss) {
          actions.push_back ( AntActionWidget::DROP_FOOD );
          actions.push_back ( AntActionWidget::DROP_WEAPONS );
          actions.push_back ( AntActionWidget::DISMISS );
        }
        AntHouse *house=dynamic_cast<AntHouse*> ( targetBoss );
        if ( house )
        {
          actions.push_back ( AntActionWidget::INVENT );
        }
        else
        {
          // actions.push_back ( AntActionWidget::FIGHT );
        }
      }
      else
      {
        actions.push_back ( AntActionWidget::FIGHT );
      }
    }
    else
    {
      AntAnimal *targetAnimal=dynamic_cast<AntAnimal*> ( target );
      if ( targetAnimal )
        actions.push_back ( AntActionWidget::FIGHT );
      AntSack *targetSack=dynamic_cast<AntSack*> ( target );
      if ( targetSack ) {
        actions.push_back ( AntActionWidget::TAKE_FOOD );
      }
    }

  } else {
    actions.push_back ( AntActionWidget::MOVE_TO );
  }
  return actions;
}

void AntGameApp::actionClicked ( AntActionWidget::Action action )
{
  actionWidget->hide();
  std::cout<<"Action clicked:"<<action<<std::endl;
  AntHero *hero=getCurrentHero();
  AntEntity *target=getSelectedEntity();
  AntBoss *targetBoss=dynamic_cast<AntBoss*> ( target );
  AntAnimal *targetAnimal=dynamic_cast<AntAnimal*> ( target );
  if ( hero && target )
  {

    switch ( action )
    {
      case AntActionWidget::FIGHT:
        {
          if ( targetBoss )
          {
            std::cout<<"Fight other boss"<<std::endl;
            hero->setHlJob ( new AntHlJobFighting ( hero,targetBoss ) );
          }
          else if ( targetAnimal )
          {
            hero->setHlJob ( new AntHLJobFightAnimal ( hero, targetAnimal ) );
            // TODO
          }
        }
        break;
      case AntActionWidget::DROP_FOOD:
        {
          if ( targetBoss )
            hero->setHlJob ( new AntHLJobDrop ( targetBoss,AntHLJobDrop::DROP_FOOD ) );
        }
        break;
      case AntActionWidget::DROP_WEAPONS:
        {
          if ( targetBoss )
            hero->setHlJob ( new AntHLJobDrop ( targetBoss,AntHLJobDrop::DROP_WEAPONS ) );
        }
        break;
      case AntActionWidget::TAKE_FOOD:
        {
          if ( targetBoss )
            hero->setHlJob ( new AntHLJobPickupFrom (hero, targetBoss,"food" ) );
        }
        break;
      case AntActionWidget::TAKE_WEAPONS:
        {
          if ( targetBoss )
            hero->setHlJob ( new AntHLJobPickupFrom (hero, targetBoss,"weapons" ) );
        }
        break;
      case AntActionWidget::DISMISS:
        {
          if ( targetBoss ) {
            std::cout<<"recruit..."<<std::endl;
            hero->setHlJob ( new AntHLJobDismiss ( hero ) );
          }
        }
        break;
      case AntActionWidget::RECRUIT:
        {
          if ( targetBoss ) {
            std::cout<<"recruit..."<<std::endl;
            hero->setHlJob ( new AntHlJobRecruit ( hero,targetBoss ) );
          }
        }
        break;
    }
  } else if(hero) {
    if ( action == AntActionWidget::MOVE_TO)
      getCurrentHero()->setHlJob ( new AntHLJobMoving ( getCurrentHero(),targetPos,0 ) );
  }



}

bool AntGameApp::eventKeyDown(AGEvent *e) {
  if(e->isSDLEvent())
  {
    if(e->getKey()==SDLK_PLUS) {
      speed*=2;
      return true;
    }
    else if(e->getKey()==SDLK_MINUS) {
      speed/=2;
      return true;
    }
    else if(e->getKey()==SDLK_1) {
      speed=1;
      return true;
    }
  }
  return AntBasicGameApp::eventKeyDown(e);
}

void AntGameApp::tryQuit() {
  AntBasicGameApp::tryQuit();

  mMap->saveMap ("tempSaved");

}

void AntGameApp::eventCameraMoved() {
    if(actionWidget)
      actionWidget->hide();
}
