#include "ant_fire.h"
#include "map.h"
#include "ant_particle.h"

AntFire::AntFire(AntMap *pMap):AntEntity(pMap) {

  AGVector3 basePoint(0,0,0);
  //setMesh(:on);
  AntParticle *smokeMesh=new AntParticle(pMap->getScene(),4);
  addMesh(smokeMesh,basePoint);
  AntParticle *smoke=new AntParticle(pMap->getScene(),40);
  smoke->setFire(true);
  smoke->setMaxTime(0.8);
  addMesh(smoke,basePoint);
  enabled=true;
}

AntFire::~AntFire() throw(){
}

void AntFire::eventNoJob() {
  if(enabled) {
    newRestJob(30);
  }  else {
    getMap()->removeEntity(this);
  }

}
