#include "ant_person.h"
#include "resource.h"
#include <map.h>
#include "ant_grave.h"
#include "ant_hero.h"
#include "ant_sack.h"

AntPerson::AntPerson(AntMap *pMap):AntEntity(pMap) {
  mOnWater=false;
}

AntPerson::~AntPerson() throw()
{

}


AGString AntPerson::checkOnWater ( const AGString& pname ) {
  AGString name=pname;
  if ( isOnWater() ) {
    // under water
    if ( name=="sit" && !isOnOpenWater() ) {
      name="stand";
      setOnWater ( false );
    }

    if ( isOnOpenWater() && haveBoat() ) {
      setOnWater ( true );
      name="row";
    }
  } else {
    setOnWater ( false );
  }
  return name;
}

void AntPerson::setOnWater ( bool f ) {
  mOnWater=f;
}

bool AntPerson::haveBoat() {
  return resource.get ( "boat" ) >=1;
}

bool AntPerson::isOnOpenWater() {
  AGVector2 p=getPos2D();
  AGVector3 p3=getMap()->getPos ( p );
  return p3.getZ() <-0.2;
}
bool AntPerson::isOnWater() {
  AGVector2 p=getPos2D();
  AGVector3 p3=getMap()->getPos ( p );
  return p3.getZ() <0;

}


void AntPerson::simDeath()
{
  playSound("die");

  // add grave
  AntGrave *grave=new AntGrave(getMap(),dynamic_cast<AntHero*>(this)?AntGrave::HERO:AntGrave::NORMAL);
  grave->init();
  grave->setPos(getPos2D());
  getMap()->insertEntity(grave);

  //# remove myself
  getMap()->removeEntity(this);

  //# add sack if resources not empty
  if (!resource.empty()) {
    AntSack *sack=new AntSack(getMap());
    sack->init();
    sack->setPos(getPos2D()+AGVector2(0.3,-0.3));
    getMap()->insertEntity(sack);
    sack->resource.takeAll(resource);
    sack->resourceChanged();
  }

}

void AntPerson::checkResources()
{
  //# FIXME: maybe make hero a little stronger ???
  //#        or even make experienced men stronger ???
  if (resource.get("bow")>0)  {
    setStrength(0.03);
    setMoraleStrength(0.04);
  } else if (resource.get("sword")>0) {
    setStrength(0.024);
    setMoraleStrength(0.03);
  } else {
    setStrength(0.015);
    setMoraleStrength(0.02);
  }
  if (resource.get("shield")==0) {
    setDefense(1);
  } else {
    setDefense(1.5);
  }

}

void AntPerson::sitDown()
{
  newRestJob(0.4);
  setMeshState("sitdown");
}


AntPerson::JobMode AntPerson::getMode()
{
  return mMode;
}
void AntPerson::setMode(AntPerson::JobMode mode)
{
  mMode=mode;
}



