#ifndef ANT_MILL_H
#define ANT_MILL_H

#include "ant_house.h"

class AntFarm:public AntMill {
public:
    AntFarm(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antMill";
    }
    void assignJob ( AntPerson *person );

    void addField(int id);
    void removeField(int id);
    std::vector<AntStockNeed > neededStock();
private:
    std::set<int> mFields;
};
#endif

