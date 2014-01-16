#include "ant_formation_singlepos.h"

AntFormationSinglePos::AntFormationSinglePos(AntBoss *pboss):AntFormation(pboss) {
}

void AntFormationSinglePos::sortMen(std::vector<AntPerson*> &tosort) {
}
std::map<AntPerson*,AGVector2> AntFormationSinglePos::computeFormation(){
  std::map<AntPerson*,AGVector2>  m;

    for(auto person:getSortedMen()) {
      m.insert(std::make_pair(person,AGVector2()));
    }

  return m;
}

