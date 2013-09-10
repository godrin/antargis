#ifndef __ANT_FIRE_H
#define __ANT_FIRE_H

#include <entity.h>

#include <rk_string.h>
#include <ag_geometry.h>

class AntMap;
class Resource;
class AntParticle;

class AntFire:public AntEntity {
public:
    AntFire(AntMap *pMap);
    virtual ~AntFire() throw();

    virtual void init();

    void eventNoJob();
    void disable();
private:
   bool enabled;
   AntParticle *smokeMesh,*fireMesh;
};

#endif
