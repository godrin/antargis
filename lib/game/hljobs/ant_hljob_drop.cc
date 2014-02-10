#include "ant_hljob_drop.h"
#include "ant_boss.h"
#include "ant_man.h"
#include "ant_sack.h"
#include "map.h"

AntHLJobDrop::AntHLJobDrop(AntBoss *pBoss, DropType pDropType):AntHLJobGatherAndDo(pBoss){
  mDropType=pDropType;
}


void AntHLJobDrop::dropSomething() {

  // FIXME: how much ???

  std::vector<AGString> a;
  if(mDropType==DROP_WEAPONS) {
    a.push_back("boat");
    a.push_back("shield");
    a.push_back("sword");
  } else {
    a.push_back("food");
  }

  AntSack *sack=0;

  for(AGString resType:a) {
    int amount=0;
    auto entities=getBoss()->getMenWithBoss();
    for(auto entity:entities) {
      amount+=entity->resource.get(resType);
      entity->resource.set(resType,0);
      entity->delJob();
    }

    int needed=entities.size()*(getBossEntity()->getAggression()/3.0f);

    int keep=std::min(needed,amount);
    getBossEntity()->resource.set(resType,keep);
    int rest=amount-keep;
    if(rest>0) {
      if(!sack)
        sack=new AntSack(getMap());
      sack->init();
      sack->setPos(getBossEntity()->getPos2D());
      getMap()->insertEntity(sack);
      sack->resource.set(resType,rest);
    }
  }
}

void AntHLJobDrop::afterGathering() {
  dropSomething();
}
    
AGString AntHLJobDrop::xmlName() const {
  return "hljobDrop";
}
void AntHLJobDrop::saveXML(Node &node) const {
  AntHLJob::saveXML(node);
  node.set("dropType",mDropType);
}
void AntHLJobDrop::loadXML(const Node &node) {
  AntHLJob::loadXML(node);
  mDropType=(DropType)node.get("dropType").toInt();
}
    
