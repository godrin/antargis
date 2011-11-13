#ifndef __ANT_HERO_H
#define __ANT_HERO_H

#include "ant_boss.h"
#include "ant_person.h"

class AntHero:public AntPerson,public AntBoss {
public:
    AntHero ( AntMap* pMap );
    virtual ~AntHero() throw();

    void init();

    virtual void setupMesh();
    virtual void setupRing();

    // wrapper methods
    virtual AntMap *getMap();
    AGVector2 getPos2D() const;
    virtual Resource &getResources();
    virtual void playSound(const AGString &psoundName);
    virtual void removeMeFromMap();
    virtual void setStrength(float f);
    virtual void setMoraleStrength(float f);
    virtual void setDefense(float f);

    void setMeshState(const AGString &p);


    virtual void saveXML ( Node &node ) const;
    virtual void loadXML ( const Node &node );

    bool isPrimary() const;
private:


    AGString appearance;
    AGString meshState;
    float age;
    bool primary;

};

#endif
