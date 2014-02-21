#include <ant_mill.h>
#include <ant_models.h>

AntMill::AntMill(AntMap* pMap): AntHouse(pMap)
{

}
void AntMill::init()
{
  AntHouse::init();
  setProvide("flour",true);
}

void AntMill::setupMesh()
{
  setMesh("mill");
}

std::vector<AntStockNeed > AntMill::neededStock() {
  std::vector< AntStockNeed > a;
  a.push_back(AntStockNeed("food",20,0));
  a.push_back(AntStockNeed("corn",30,0));
  return a;
}
void AntMill::process() {
  if( resource.get("corn")>0 && resource.get("flour")<30) {
    auto amount=std::min(resource.get("corn"),1.0f);
    resource.add("flour",amount);
    resource.sub("corn",amount);
  }
}

