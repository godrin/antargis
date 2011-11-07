#include "ant_tree.h"
#include "ant_models.h"

AntTree::AntTree ( AntMap* pMap ) : AntEntity ( pMap ) {

}

void AntTree::init() {
    AntEntity::init();
    
    setMesh(AntModels::createModel(getScene(),"tree","2"));
}
