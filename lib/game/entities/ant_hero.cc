#include "ant_hero.h"

AntHero::AntHero ( AntMap* pMap ) : AntBoss ( pMap ) {

}

AntHero::~AntHero() throw() {

}

void AntHero::setupMesh()
{
setMeshState("normal");
}

