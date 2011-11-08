#ifndef __ANT_HERO_H
#define __ANT_HERO_H

#include "ant_boss.h"
#include "ant_person.h"

class AntHero:public AntPerson,public AntBoss {
    public:
        AntHero ( AntMap* pMap );
        virtual ~AntHero() throw();

        virtual void setupMesh();
        virtual void setupRing();

        virtual AntMap *getMap();
        AGVector2 getPos2D() const;
        virtual Resource &getResources();

        virtual void saveXML ( Node &node ) const;
        virtual void loadXML ( const Node &node );
        
        bool isPrimary() const;

    private:

        void setMeshState ( const AGString &s );

        AGString appearance;
        AGString meshState;
        float age;
        bool primary;

};

#endif
