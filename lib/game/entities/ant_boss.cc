#include "ant_boss.h"

AntBoss::AntBoss ( AntMap* pMap ) : AntEntity ( pMap ) {
}

AntBoss::~AntBoss() throw() {

}

void AntBoss::setupMeshBoss() {
    setupMesh();
    setupRing();
}

void AntBoss::init() {
    AntEntity::init();
    setupMeshBoss();
}
