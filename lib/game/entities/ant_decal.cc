#include "ant_decal.h"

AntDecal::AntDecal ( AntMap* pMap ):AntEntity(pMap) {
  typeID="0";
}

AntDecal::~AntDecal() throw() {
}

void AntDecal::init() {
  AntEntity::init();

  setMesh("floor_gravel");
}

void AntDecal::saveXML ( Node &node ) const {
  AntEntity::saveXML ( node );

  node.set ( "typeID",typeID );
}

void AntDecal::loadXML ( const Node &node ) {
  AntEntity::loadXML ( node );

  typeID=node.get ( "typeID" );
}


