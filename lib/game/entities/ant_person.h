#ifndef __ANT_PERSON_H
#define __ANT_PERSON_H

#include <entity.h>

#include <rk_string.h>
#include <ag_geometry.h>

class AntMap;
class Resource;

class AntPerson:public AntEntity {
public:
    enum JobMode {WAITING,FIGHTING,MOVING,REST_SIT,REST_EAT,FORMAT,READY,FETCHING,HOMING,DIGGING,INVALID,MAKE_FIELD};

    AntPerson(AntMap *pMap);
    virtual ~AntPerson() throw();
    virtual bool isOnWater() ;
    virtual bool isOnOpenWater() ;

    virtual void setMeshState(const AGString &p)=0;
    virtual const AGString &getMeshState() const=0;

    virtual bool haveBoat() ;
    AGString checkOnWater ( const AGString &name );
    void setOnWater ( bool f );
    void checkResources();

    void sitDown();
    void standUp();

    void setMode(JobMode mode);
    JobMode getMode();
    void newFightJob ( int p,AntEntity *target );

    AGString getModeString() const;

protected:
    virtual void simDeath();
    virtual void eventStartFighting();
    virtual void eventStartMoving();
private:
    AGString getWeapon();

    JobMode mMode;
    bool mOnWater;
};

#endif
