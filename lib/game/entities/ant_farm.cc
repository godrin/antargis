#include <ant_farm.h>
#include <ant_models.h>
#include <map.h>
#include <ant_field.h>
#include <ag_main.h>

AntFarm::AntFarm(AntMap* pMap): AntHouse(pMap)
{

}
void AntFarm::init()
{
  AntHouse::init();
  setProvide("farm",true);
}

void AntFarm::setupMesh()
{
  setMesh("farm");
}

void AntFarm::addField(int id) {
  mFields.insert(id);
}
void AntFarm::removeField(int id) {
  mFields.erase(id);
  getMap()->removeEntity(getMap()->getEntity(id));
}

void AntFarm::assignJob ( AntPerson *person ) {
  CTRACE;
  if (!isFighting()) {
    if(person->getMode()==AntPerson::MAKE_FIELD) {
      auto pos=person->getPos2D();
      auto field=new AntField(getMap());
      field->init();
      field->setPos(pos);
      getMap()->insertEntity(field);
      cdebug("INSERT ENT "<<mFields.size());
      mFields.insert(field->getID());
      field->setFarmId(getID());
      person->setMode(AntPerson::HOMING);
      person->newMoveJob(0,getPos2D(),0);
      return;
    }
    else if(mFields.size()<7) {
      person->setMode(AntPerson::MAKE_FIELD);
      auto angle=(*getMain()->getRand())(360.0f); //->getRand()*360;
      auto radius=(*getMain()->getRand())(3.0f)+6; //->getRand()*360;
      person->newMoveJob(0,getPos2D()+AGVector2(AGAngle(angle))*radius,0);
      return;
    }
  }
  AntHouse::assignJob(person);
}
std::vector<AntStockNeed > AntFarm::neededStock() {
  std::vector< AntStockNeed > a;
  a.push_back(AntStockNeed("fruit",15,0));
  a.push_back(AntStockNeed("crop",15,0));
  return a;
}
void AntFarm::process() {
  if( resource.get("fruit")>0 && resource.get("food")<15) {
    auto amount=std::min(resource.get("fruit"),3.0f);
    resource.add("food",amount);
    resource.sub("fruit",amount);
  }
  if (resource.get("crop")>0 && resource.get("corn")<15) {
    auto amount=std::min(resource.get("crop"),3.0f);
    resource.add("corn",amount);
    resource.sub("crop",amount);
  }
}
