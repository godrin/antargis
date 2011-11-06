#ifndef __ANT_HERO_H
#define __ANT_HERO_H

#include "ant_boss.h"
#include "ant_person.h"

class AntHero:public AntBoss,public AntPerson {
    public:
        AntHero ( AntMap* pMap );
        virtual ~AntHero() throw();

        virtual void setupMesh();
        virtual void setupRing();

    private:
        AGString appearance;
        AGString meshStand;
        float age;

};

#endif
