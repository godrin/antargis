#include <ant_bush.h>
#include <mesh.h>

AntBush::AntBush ( AntMap* pMap ) : AntEntity ( pMap ) {

}

void AntBush::init() {
    AntEntity::init();
    setProvide ( "bush",true );
    size= ( rand() %10 ) /10.0*0.25+0.25;
    setupMesh();
}

void AntBush::setupMesh() {
  setMesh("bush","",size);
}

