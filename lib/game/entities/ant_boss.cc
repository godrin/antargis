#include "ant_boss.h"

AntBoss::AntBoss ( AntMap* pMap ) : AntEntity ( pMap ) {
    setupMeshBoss();
}

AntBoss::~AntBoss() throw() {

}

void AntBoss::setupMeshBoss() {
    setupMesh();
    setupRing();
}
