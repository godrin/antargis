#ifndef __ANT_BOSS_H
#define __ANT_BOSS_H

#include "entity.h"

class AntMan;
class AntHero;
class AntPerson;
class AntHLJob;
class AntFormation;
class AntPlayer;

class AntBoss
{
public:
    AntBoss();
    virtual ~AntBoss() throw ();

    void setupMeshBoss();
    virtual void setupMesh() =0;
    virtual void setupRing() =0;

    virtual void init();

    void signUp ( AntMan*man );
    void removeMan ( AntMan*man );

    void assignJob ( AntMan *man );
    void assignJob ( AntHero *man );

    virtual AntEntity *getEntity() =0;

    AntFormation *getFormation();
    AGVector2 getFormation ( AntPerson*e,const AGVector2 &v );
    std::vector<AntPerson*> getMenWithoutBoss();
    std::vector<AntPerson*> getMenWithBoss();

    virtual void eventNoHlJob() =0;

    void setHlJob ( AntHLJob *job );
    void setFormation ( AntFormation *formation );

    void setPlayer ( AntPlayer *player );
    AntPlayer *getPlayer();


protected:
    void checkHlJobEnd();

    void loadXMLBoss ( const Node &node );

private:
    std::list<AntMan*> menWithoutBoss;
    AntHLJob *hlJob;
    AntFormation *formation;
    AntPlayer *player;
    
    size_t menToAddCount;
};

#endif
