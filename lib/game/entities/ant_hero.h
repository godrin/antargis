#ifndef __ANT_HERO_H
#define __ANT_HERO_H

#include "ant_boss.h"
#include "ant_person.h"
#include "ag_texture.h"

class AntFire;


class AntHero:public AntPerson,public AntBoss {
  public:
    static const AGVector2 FIRE_DISPLACE;

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

    virtual AGString xmlName() const {
      return "antHero";
    }

    AntMap *getMap();
    int getID();

    void setFire(bool flag);

    void setHlJob ( AntHLJob *job );

    AGTexture getImage();
  private:

    AGString appearance;
    AGString meshState;
    float age;
    bool primary;

    int fireID;
};

#endif
