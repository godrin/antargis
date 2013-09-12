#ifndef __ANT_HERO_H
#define __ANT_HERO_H

#include "ant_boss.h"
#include "ant_person.h"

class AntFire;


class AntHero:public AntPerson,public AntBoss {
  public:
    AntHero ( AntMap* pMap );
    virtual ~AntHero() throw();

    void init();

    virtual void setupMesh();
    virtual void setupRing();

    void setMeshState(const AGString &p);
    const AGString &getMeshState() const;

    AntEntity *getEntity();

    virtual void saveXML ( Node &node ) const;
    virtual void loadXML ( const Node &node );

    virtual void eventNoHlJob();
    virtual void eventNoJob();
    bool isPrimary() const;

    AntMap *getMap();
    int getID();

    void setFire(bool flag);

    void setHlJob ( AntHLJob *job );
  private:

    AGString appearance;
    AGString meshState;
    float age;
    bool primary;

    AntFire *fire;
};

#endif
