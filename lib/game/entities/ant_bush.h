#ifndef __ANT_BUSH_H
#define __ANT_BUSH_H


#include "entity.h"

class AntBush:public AntEntity {
    public:
        AntBush ( AntMap* pMap );
        virtual void init();
    private:
        void setupMesh();

        float size;
};

#endif
