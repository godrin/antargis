#ifndef __ANT_HOUSE_H
#define __ANT_HOUSE_H

#include <entity.h>
#include <ant_boss.h>

#include <set>

class AntMan;

class AntHouse:public AntEntity,public AntBoss
{
  public:
    AntHouse ( AntMap* pMap );
    virtual ~AntHouse() throw();

    virtual void init();
    virtual void removeMan ( AntMan *man );

    virtual ColoredMesh *getRing();
    virtual void setupRing();
    virtual void saveXML ( Node &node )const ;
    virtual void loadXML ( const Node &node );

    AntEntity *getEntity();

    virtual void eventNoHlJob();
    void eventNoJob();
    void setHlJob ( AntHLJob* job );

    AntMap *getMap();
    int getID();

    virtual void process();
  private:
    std::set<AntMan*> atHome;

};

#endif
