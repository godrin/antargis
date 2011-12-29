#include "ant_ring.h"
#include "map.h"
#include "mesh.h"
#include "mesh_data.h"
#include "mesh_optimizer.h"
#include "ant_colored_mesh.h"

#include <cmath>

std::map<float,MeshData*> ringData;

MeshData *getRingData ( float w=1.0 ) {

    if ( ringData.find ( w ) !=ringData.end() )
        return ringData[w];

    MeshOptimizer opt;
    MeshVertex mv0,mv1,mv2,mv3;

    AGVector4 c ( 1,1,1,1 );
    mv0.c=c;
    mv1.c=c;
    mv2.c=c;
    mv3.c=c;

    mv0.t=AGVector2 ( 0,0 );
    mv1.t=AGVector2 ( 1,0 );
    mv2.t=AGVector2 ( 1,1 );
    mv3.t=AGVector2 ( 0,1 );

//#w=size
    float h=0.1;

    mv0.v=AGVector4 ( -w,-w,h );
    mv1.v=AGVector4 ( w,-w,h );
    mv2.v=AGVector4 ( w,w,h );
    mv3.v=AGVector4 ( -w,w,h );

    mv0.n=AGVector3 ( 0,-1,0 );
    mv1.n=AGVector3 ( 0,-1,0 );
    mv2.n=AGVector3 ( 0,-1,0 );
    mv3.n=AGVector3 ( 0,-1,0 );

    opt.add ( mv0 );
    opt.add ( mv1 );
    opt.add ( mv2 );

    opt.add ( mv0 );
    opt.add ( mv2 );
    opt.add ( mv3 );

    ringData[w]=new MeshData ( opt.getArray(),"data/textures/ring.png",false );
    ringData[w]->setOverdraw ( true );
    ringData[w]->setColors ( false );
    ringData[w]->setPickable ( false );
    ringData[w]->setLighting ( false );
    return ringData[w];
}


ColoredMesh* makeRingMesh ( AntMap *map ) {
    if ( !map->getScene() )
        return 0;
    ColoredMesh *mesh=new ColoredMesh ( dynamic_cast<Scene*> ( map->getScene() ),getRingData(),AGVector4 ( 0,0,0,0 ),0 );
    mesh->setOrder ( RING_Z );
    return mesh;
}
ColoredMesh* makeBigRingMesh ( AntMap *map ) {
    if ( !map->getScene() )
        return 0;
    ColoredMesh *mesh=new ColoredMesh ( dynamic_cast<Scene*> ( map->getScene() ),getRingData ( 4 ),AGVector4 ( 0,0,0,0 ),0 );
    mesh->setOrder ( RING_Z );
    return mesh;
}
/*
def makeBigRingMesh
  return nil if getMap.getScene.nil?
  return AntModels.createModel(:sack) if not opengl # FIXME
  mesh=ColoredMesh(getMap.getScene,RingData.getRingData(4),AGVector4(0,0,0,0),0)
  mesh.setOrder(RING_Z)
  return mesh
end



}
*/
