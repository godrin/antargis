#include "ant_fire.h"
#include "map.h"
#include "ant_particle.h"
#include "ant_models.h"
#include "ant_sound.h"

AntFire::AntFire(AntMap *pMap):AntEntity(pMap) {
  CTRACE;
}

void AntFire::init() {
  CTRACE;
  AntEntity::init();
  AGVector3 basePoint(0,0,0);
  setMesh("fire","on");
  enabled=true;
}

AntFire::~AntFire() throw(){
  AntSound::stopLoopSound(getID());
}

void AntFire::eventNoJob() {
  if(enabled) {
    AntSound::playLoopSoundGlobal(getID(),"fire",getPos2D(),0.4);
    newRestJob(30);
  }  else {
    getMap()->removeEntity(this);
  }
}

void AntFire::disable() {
  CTRACE;
  enabled=false;
  setMesh("fire","off");
  AntSound::stopLoopSound(getID());
}
