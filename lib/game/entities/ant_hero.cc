#include "ant_hero.h"
#include "ant_models.h"
#include "map.h"

AntHero::AntHero ( AntMap* pMap ) : AntBoss ( pMap ) {

}

AntHero::~AntHero() throw() {

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


AntMap* AntHero::getMap() {
    return AntBoss::getMap();
}

AGVector2 AntHero::getPos2D() const {
    return AntBoss::getPos2D();
}

void AntHero::setupRing() {

}

Resource& AntHero::getResources() {
    return resource;
}

void AntHero::loadXML ( const Node& node ) {
    AntEntity::loadXML ( node );
    primary= ( AGString ( "true" ) ==node.get ( "primary" ) );
    std::cout<<"PRIMARAY:"<<(primary?"true":"false")<<std::endl;
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

void AntHero::playSound(const AGString& psoundName)
{
    AntEntity::playSound(psoundName);
}

void AntHero::removeMeFromMap()
{
    getMap()->removeEntity(this);
}


void AntHero::setDefense(float f)
{
    AntEntity::setDefense(f);
}
void AntHero::setMoraleStrength(float f)
{
    AntEntity::setMoraleStrength(f);
}

void AntHero::setStrength(float f)
{
    AntEntity::setStrength(f);
}

