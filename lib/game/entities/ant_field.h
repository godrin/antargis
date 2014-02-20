#ifndef __ANT_FIELD_H
#define __ANT_FIELD_H

#include "entity.h"

class AntFarm;

class AntField:public AntEntity {
public:
    AntField(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antField";
    }
    void setFarmId(int farmId);
    void resourceChanged();
    virtual void eventNoJob();
private:

    AntFarm *getFarm();
    int mFarmId;
    int mSize;
    int mGrowTime;
    bool mBegin;
    int mMax;
};

#endif
