#ifndef __ANT_BUSH_H
#define __ANT_BUSH_H


#include "entity.h"

class AntBush:public AntEntity {
    public:
        AntBush ( AntMap* pMap );
        virtual void init();
        virtual AGString xmlName() const {
          return "antBush";
        }
    private:
        void setupMesh();

        float size;
};

#endif
