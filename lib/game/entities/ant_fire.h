#ifndef __ANT_FIRE_H
#define __ANT_FIRE_H

#include <entity.h>

#include <rk_string.h>
#include <ag_geometry.h>

class AntMap;
class Resource;

class AntFire:public AntEntity {
public:
    AntFire(AntMap *pMap);
    virtual ~AntFire() throw();

    void eventNoJob();
private:
   bool enabled;
};

#endif
