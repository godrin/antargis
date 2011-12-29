#ifndef __ANT_PERSON_H
#define __ANT_PERSON_H

#include <entity.h>

#include <rk_string.h>
#include <ag_geometry.h>

class AntMap;
class Resource;

class AntPerson:public AntEntity {
public:
    enum JobMode {WAITING,FIGHTING,MOVING,REST_SIT,REST_EAT,FORMAT,READY,FETCHING,HOMING,DIGGING};
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

    void setMode(JobMode mode);
    JobMode getMode();

protected:
    virtual void simDeath();
private:

    JobMode mMode;
    bool mOnWater;
};

#endif
