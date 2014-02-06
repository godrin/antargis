#ifndef __ANT_MAN_H
#define __ANT_MAN_H

#include "entity.h"
#include "ant_person.h"

class AntBoss;

class AntMan:public AntPerson {
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

    void setMeshState(const AGString &p);

    void loadXML(const Node &n);
    virtual void saveXML(Node &n) const;

    virtual AGString xmlName() const {
      return "antMan";
    }

    const AGString &getMeshState() const;

    void setBoss(AntBoss *pBoss);
    AntBoss* getBoss();

    void setFetchResource(const AGString &r);
    AGString getFetchResource() const;
    void digResource(const AGString &resource);
    void collectResource(const AGString &resource);
    float canCarry() const;


protected:
    void eventNoJob();
    void eventDefeated();
    void eventMoraleLow();
private:

    void leaveBoss();

    void simDeath();
    AGString getWeapon();

    AGString meshState;
    AntBoss *boss;
    AGString bossName;
    float age;
    bool dead;
    bool fighting;
    AGString fetchResource;
};

#endif
