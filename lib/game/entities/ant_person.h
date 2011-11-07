#ifndef __ANT_PERSON_H
#define __ANT_PERSON_H

#include <rk_string.h>
#include <ag_geometry.h>

class AntMap;
class Resource;

class AntPerson {
    public:
      AntPerson();
        virtual bool isOnWater() ;
        virtual bool isOnOpenWater() ;
        virtual AGVector2 getPos2D() const=0;
        virtual AntMap *getMap() =0;
        virtual bool haveBoat() ;

        AGString checkOnWater ( const AGString &name );
        virtual Resource &getResources()=0;

        void setOnWater ( bool f );
    private:
        bool mOnWater;
};

#endif
