#ifndef __ANT_MAN_H
#define __ANT_MAN_H

#include "entity.h"
#include "ant_person.h"

class AntBoss;

class AntMan:public AntEntity,public AntPerson {
public:
    AntMan(AntMap* pMap);
    virtual ~AntMan() throw();

    // wrapper methods
    virtual AntMap *getMap();
    AGVector2 getPos2D() const;
    virtual Resource &getResources();
    virtual void playSound(const AGString&soundName);
    virtual void removeMeFromMap();
    virtual void setStrength(float f);
    virtual void setMoraleStrength(float f);
    virtual void setDefense(float f);

    void init();

    void setFighting(bool flag);
protected:
    void eventNoJob();
private:

    void setMeshState(const AGString &p);
    void simDeath();
    AGString getWeapon();

    AntBoss *boss;
    AGString bossName;
    enum JobMode {WAITING,FIGHTING,MOVING};
    JobMode mode;
    float age;
    bool dead;
    bool fighting;
};

#endif
