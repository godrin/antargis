#include <ant_dwelling.h>
#include <ant_models.h>
#include <map.h>
#include <ant_man.h>

#define BIRTHING_TIME 80
#define MAX_RESIDENTS 10

AntDwelling::AntDwelling(AntMap* pMap): AntHouse(pMap)
{

}
void AntDwelling::init()
{
  AntHouse::init();
  lastBirth=0;
}

void AntDwelling::setupMesh()
{
  setMesh("dwelling");
}

std::vector<AntStockNeed > AntDwelling::neededStock() {
  std::vector< AntStockNeed > a;
  a.push_back(AntStockNeed("food",15,0));
  return a;
}
void AntDwelling::process() {
  //# only men, which are at home can add to birth rate
  lastBirth+=std::min(atHome()+resource.get("food")/3.0,10.0);
  if(lastBirth>BIRTHING_TIME) {
    auto man=new AntMan(getMap());
    getMap()->insertEntity(man);
    man->setPos(getPos2D());
    man->setBoss(this);
    lastBirth=-BIRTHING_TIME;
  }

  while( getMenWithoutBoss().size()>MAX_RESIDENTS) {
    int max=4;
    for(auto person:getMenWithoutBoss()) {
      auto man=dynamic_cast<AntMan*>(person);
      if(man) {
        man->setBoss(0);
        max--;
        if(max<0)
          break;
      }
    }
  }
}
