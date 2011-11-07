#ifndef __ANT_TREE_H
#define __ANT_TREE_H

#include "entity.h"

class AntTree:public AntEntity {
public:
        AntTree ( AntMap* pMap );
        
        virtual void init();
};

#endif
