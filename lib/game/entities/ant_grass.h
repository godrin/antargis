#ifndef __ANT_GRASS_H
#define __ANT_GRASS_H

#include "entity.h"

class AntGrass:public AntEntity {
    public:
        AntGrass ( AntMap* pMap,bool ishigh=false );
        virtual void init();
    private:
        void setupMesh();

        float size;
        bool high;
};



#endif

