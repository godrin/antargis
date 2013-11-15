#include "ant_hljob_fighting.h"

AntHlJobFighting::AntHlJobFighting(AntBoss* pBoss,AntBoss *pTarget): AntHLJobMoving(pBoss),target(pTarget)
{

}


AGString AntHlJobFighting::xmlName() const {
  return "hljobFighting";
}
void AntHlJobFighting::saveXML(Node &node) const {
}
void AntHlJobFighting::loadXML(const Node &node){
}
