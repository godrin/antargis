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

    // pure virtual methods, that are in AntEntity
    virtual AGVector2 getPos2D() const=0;
    virtual AntMap *getMap() =0;
    virtual Resource &getResources()=0;
    virtual void playSound(const AGString&soundName)=0;
    virtual void removeMeFromMap()=0;
    virtual void setStrength(float f)=0;
    virtual void setMoraleStrength(float f)=0;
    virtual void setDefense(float f)=0;

    virtual void setMeshState(const AGString &p)=0;


    virtual bool haveBoat() ;
    AGString checkOnWater ( const AGString &name );
    void setOnWater ( bool f );
    void checkResources();
protected:
    virtual void simDeath();
private:
    bool mOnWater;
};

#endif
