#ifndef __ANT_MAN_H
#define __ANT_MAN_H

#include "entity.h"
#include "ant_person.h"

class AntBoss;

class AntMan:public AntPerson {
public:
    enum JobMode {WAITING,FIGHTING,MOVING,REST_SIT,REST_EAT};

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

    void setMeshState(const AGString &p);

    void loadXML(const Node &n);

    void setMode(JobMode mode);
    JobMode getMode();
    
    const AGString &getMeshState() const;

protected:
    void eventNoJob();
private:

    void simDeath();
    AGString getWeapon();

    AGString meshState;
    AntBoss *boss;
    AGString bossName;
    JobMode mMode;
    float age;
    bool dead;
    bool fighting;
};

#endif
