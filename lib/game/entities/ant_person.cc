#include "ant_person.h"
#include "resource.h"
#include <map.h>


AntPerson::AntPerson() {
    mOnWater=false;
}


AGString AntPerson::checkOnWater ( const AGString& pname ) {
    AGString name=pname;
    if ( isOnWater() ) {
        // under water
        if ( name=="sit" && !isOnOpenWater() ) {
            name="stand";
            setOnWater ( false );
        }

        if ( isOnOpenWater() && haveBoat() ) {
            setOnWater ( true );
            name="row";
        }
    } else {
        setOnWater ( false );
    }
    return name;
}

void AntPerson::setOnWater ( bool f ) {
    mOnWater=f;
}

bool AntPerson::haveBoat() {
    return getResources().get ( "boat" ) >=1;
}

bool AntPerson::isOnOpenWater() {
    AGVector2 p=getPos2D();
    AGVector3 p3=getMap()->getPos ( p );
    return p3.getZ() <-0.2;
}
bool AntPerson::isOnWater() {
    AGVector2 p=getPos2D();
    AGVector3 p3=getMap()->getPos ( p );
    return p3.getZ() <0;

}
