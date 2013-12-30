#include "ant_tree.h"
#include "ant_models.h"

#include <algorithm>

AntTree::AntTree ( AntMap* pMap ) : AntEntity ( pMap ) {
    typeID="0";
}


AntTree::~AntTree() throw() {

}

void AntTree::init() {
    AntEntity::init();

setMesh("tree",typeID);




    setProvide ( "wood",true );
    setProvide ( "fruit",true );
    
    resource.set("fruit",5);
    resource.set("wood",20);
}
void AntTree::eventNoJob() {
    AntEntity::eventNoJob();
    resource.set ( "fruit",std::min ( resource.get ( "fruit" ) +1.7, ( double ) maxApples ) );
    resourceChanged();
    newRestJob ( 20 );
}


void AntTree::loadXML ( const Node& node ) {
    AntEntity::loadXML ( node );

    typeID=node.get ( "typeID" );
}

void AntTree::saveXML ( Node& node ) const {
    AntEntity::saveXML ( node );

    node.set ( "typeID",typeID );
}
