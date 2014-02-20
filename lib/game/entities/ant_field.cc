#include "ant_field.h"
#include "ant_farm.h"
#include "map.h"


AntField::AntField(AntMap* pMap):AntEntity(pMap),
  mFarmId(-1), mSize(0),mGrowTime(20),mBegin(true),mMax(3) {
    setProvide("field",true);

  }
void AntField::init() {
  AntEntity::init();
  newRestJob(mGrowTime);
  setupMesh();
}

void AntField::setupMesh() {
  CTRACE;
  setMesh("field",AGString(mSize));
}
void AntField::resourceChanged() {
  if(mSize==mMax)
    if(resource.get("crop")==0) {
      // all crop was harvested - remove this field
      AntFarm *farm=getFarm();
      if(farm) {
        farm->removeField(getID());
      }
    }
  setupMesh();
  AntEntity::resourceChanged();
}

void AntField::eventNoJob() {
  mSize=std::min(mMax,mSize+1);
  if(mSize==mMax && mBegin) {
    // crop is ripe, so it can be harvested
    setProvide("crop",true);
    resource.set("crop",5);
    mBegin=false;
  }
  newRestJob(mGrowTime);
  resourceChanged();
}
AntFarm *AntField::getFarm() {
  return dynamic_cast<AntFarm*>(getMap()->getEntity(mFarmId));
}


void AntField::setFarmId(int farmId) {
  mFarmId=farmId;
}
