#include <ant_grass.h>
#include <ant_grass_mesh.h>
#include <mesh.h>

AntGrass::AntGrass ( AntMap* pMap, bool ishigh ) : AntEntity ( pMap ),high ( ishigh ) {

}

void AntGrass::init() {
    AntEntity::init();
    setProvide ( "grass",true );
    if ( high )
        size= ( rand() %10 ) /10.0*0.25+0.5;
    else
        size= ( rand() %10 ) /10.0*0.25+0.25;
    setupMesh();
}

void AntGrass::setupMesh() {
    setMesh ( makeGrassMesh ( getScene(),size ) );
}

