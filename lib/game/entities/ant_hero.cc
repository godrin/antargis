#include "ant_hero.h"
#include "ant_models.h"
#include "map.h"
#include "ant_hljob_rest.h"

AntHero::AntHero ( AntMap* pMap ) : AntPerson ( pMap ) {

}

AntHero::~AntHero() throw() {

}

void AntHero::init()
{
    AntPerson::init();
    AntBoss::init();
    checkResources();
}


void AntHero::setupMesh() {
    setMeshState ( "normal" );
}


void AntHero::setMeshState ( const AGString& pname ) {
    AGString name=pname;
    if ( name=="row" ) {
        name="walk";
    }
    //origMeshState=name;
    name=checkOnWater ( name );
    meshState=name;
    float dir=getDirection();

    Scene *scene=getScene();
    if ( name=="row" ) {
        setMesh ( AntModels::createModel ( scene,"hero","" ) );
        addMesh ( AntModels::createModel ( scene,"hero","boat" ),AGVector3 ( 0,0,0 ) );
    } else if ( name=="dead" ) {
        setMesh ( AntModels::createModel ( scene,"hero","grave_hero" ) );
    } else {
        setMesh ( AntModels::createModel ( scene,"hero","" ) );
    }
    setDirection ( dir );
    setupRing();

}

const AGString &AntHero::getMeshState() const
{
    return meshState;
}




void AntHero::setupRing() {

}


void AntHero::loadXML ( const Node& node ) {
    AntEntity::loadXML ( node );
    primary= ( AGString ( "true" ) ==node.get ( "primary" ) );
    std::cout<<"PRIMARAY:"<<(primary?"true":"false")<<std::endl;
    AntBoss::loadXMLBoss(node);
}
void AntHero::saveXML ( Node& node ) const {
    AntEntity::saveXML ( node );
    if ( primary ) {
        node.set ( "primary","true" );
    }
}
bool AntHero::isPrimary() const {
    return primary;
}



AntEntity* AntHero::getEntity()
{
    return this;
}

void AntHero::eventNoHlJob()
{
    CTRACE;
    setHlJob(new AntHLJobRest(this,20));
}

void AntHero::eventNoJob()
{
    AntEntity::eventNoJob();
    checkHlJobEnd();
}

