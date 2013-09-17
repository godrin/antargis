#include "ant_fire.h"
#include "map.h"
#include "ant_particle.h"
#include "ant_models.h"

AntFire::AntFire(AntMap *pMap):AntEntity(pMap) {
  CTRACE;
}

void AntFire::init() {
  CTRACE;
  AntEntity::init();
  AGVector3 basePoint(0,0,0);
  setMesh(AntModels::createModel(getScene(),"fire",""));
  smokeMesh=new AntParticle(getMap()->getScene(),4);
  addMesh(smokeMesh,basePoint);
  fireMesh=new AntParticle(getMap()->getScene(),40);
  fireMesh->setFire(true);
  fireMesh->setMaxTime(0.8);
  addMesh(fireMesh,basePoint);
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

void AntFire::disable() {

  AGVector3 basePoint(0,0,0);
  CTRACE;
  enabled=false;
  // take particles and delete the rest of the meshes
  detachMesh(smokeMesh);
  detachMesh(fireMesh);
  setMesh(AntModels::createModel(getMap()->getScene(),"fire","off"));
  addMesh(smokeMesh,basePoint);
  addMesh(fireMesh,basePoint);
  smokeMesh->setEnabled(false);
  fireMesh->setEnabled(false);
}
