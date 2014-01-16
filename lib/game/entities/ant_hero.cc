#include "ant_hero.h"
#include "ant_models.h"
#include "map.h"
#include "ant_hljob_rest.h"
#include "ant_fire.h"
#include "ag_texturecache.h"
#include "ant_hljob_fighting.h"

#include <algorithm>

const AGVector2 AntHero::FIRE_DISPLACE(0.5,-0.5);

AntHero::AntHero ( AntMap* pMap ) : AntPerson ( pMap ) {
  fireID=-1;
}

AntHero::~AntHero() throw()
{
}

void AntHero::init()
{
  AntPerson::init();
  AntBoss::init();
  checkResources();
}


void AntHero::setupMesh()
{
  setMeshState ( "normal" );
}


void AntHero::setMeshState ( const AGString& pname )
{
  AGString name=pname;
  if ( name=="row" )
  {
    name="walk";
  }
  //origMeshState=name;
  name=checkOnWater ( name );
  meshState=name;
  float dir=getDirection();

  if ( name=="row" )
  {
    setMesh("hero","row");
 //   setMesh ( AntModels::createModel ( scene,"hero","" ) );
  //  addMesh ( AntModels::createModel ( scene,"hero","boat" ),AGVector3 ( 0,0,0 ) );
  }
  else if ( name=="dead" )
  {
    setMesh("hero","grave");
    //setMesh ( AntModels::createModel ( scene,"hero","grave_hero" ) );
  }
  else
  {
    setMesh("hero");
    //setMesh ( AntModels::createModel ( scene,"hero","" ) );
  }
  setDirection ( dir );
  setupRing();
  cdebug("setMeshState Hero"<<pname);
}

const AGString &AntHero::getMeshState() const {
  return meshState;
}

void AntHero::setupRing() {
}

void AntHero::loadXML ( const Node& node )
{
  CTRACE;
  fireID=node.get("fireID").toInt();
  cdebug("fireID:"<<fireID);

  AntEntity::loadXML ( node );
  primary= ( AGString ( "true" ) ==node.get ( "primary" ) );
  std::cout<<"PRIMARY:"<< ( primary?"true":"false" ) <<std::endl;
  std::cout<<"ID:"<<getID()<<std::endl;
  AntBoss::loadXMLBoss ( node );
}
void AntHero::saveXML ( Node& node ) const
{
  AntEntity::saveXML ( node );
  AntBoss::saveXMLBoss(node);
  if ( primary )
  {
    node.set ( "primary","true" );
  }
  if(fireID>0)
    node.set("fireID",fireID);
}
bool AntHero::isPrimary() const
{
  return primary;
}

AntEntity* AntHero::getEntity()
{
  return this;
}

void AntHero::eventNoHlJob()
{
  setHlJob ( new AntHLJobRest ( this,50 ) );
}

void AntHero::eventNoJob()
{
  //AntPerson::eventNoJob();

  checkHlJobEnd();
  assignJob ( this );
}


AntMap* AntHero::getMap()
{
  return AntPerson::getMap();
}


int AntHero::getID()
{
  return AntPerson::getID();
}


void AntHero::setFire(bool flag) {
  CTRACE;
  cdebug("FIREID:"<<fireID<<"   flag:"<<flag);
  AntFire *fire=0;
  if(fireID>0) 
    fire=dynamic_cast<AntFire*>(getMap()->getEntity(fireID));
  if(fire && !flag) {
    cdebug("FIRE:"<<getMap()->getEntity(fireID)<<"  "<<fire);
    cdebug("remove fire "<<fire);
    //getMap()->removeEntity(fire);
    fire->disable();
    fireID=0;
    fire=0;
  }

  if(flag && !fire) {
    AGVector2 firePos=getPos2D();
    firePos+=FIRE_DISPLACE;
    fire=new AntFire(getMap());
    fire->init();
    fire->setPos(firePos);
    getMap()->insertEntity(fire);
    fireID=fire->getID();
  }
}


void AntHero::setHlJob ( AntHLJob *job ) {
  AntBoss::setHlJob(job);
  if(job)
    cdebug("setHlJob:"<<job<<" "<<typeid(*job).name()<<" disable fire");
  setFire(job && job->fireBurning());
}

AGTexture AntHero::getImage() {
  AGString dir="data/gui/portraits/";
  AGString filename=dir;
  filename+=getName();
  filename+=".png";
  if(!fileExists(filename))
    filename=dir+"none.png";
  return getTextureCache()->get(filename);
}

void AntHero::eventDefeated() {
  auto fightJob=dynamic_cast<AntHlJobFighting*>(getHlJob());
  if(fightJob) {
    fightJob->reactOnLost();
  }
}
