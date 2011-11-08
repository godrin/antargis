#ifndef __ANT_TREE_H
#define __ANT_TREE_H

#include "entity.h"

class AntTree:public AntEntity {
    public:
        AntTree ( AntMap* pMap );
        virtual ~AntTree() throw();

        virtual void init();
        void eventNoJob();

        virtual void saveXML ( Node &node ) const;
        virtual void loadXML ( const Node &node );

    private:
        static const size_t maxApples=100;
        std::string typeID;
        float angle;

};

#endif
