#include "ant_ring.h"
#include "map.h"
#include "mesh.h"
#include "mesh_data.h"
#include "mesh_optimizer.h"

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

class ColoredMesh:public Mesh {
    public:
        ColoredMesh ( Scene *scene,MeshData *data,AGVector4 pos,float angle ) :
                Mesh ( scene,*data,pos,angle ) {


            t=0;
            c0=AGVector4 ( 0.6,0.6,0.6,1 );
            c1=AGVector4 ( 0.8,0.8,0.8,1 );
        }
        void setRingColor ( AGVector4 c ) {
            c0=c;
            c1=c*1.2;
            c1.setW ( 1 );
        }
        void advance ( float time ) {
            t+=time;
            float v= ( cos ( t*5 ) +1 ) /2;
            setColor ( c0*v+c1* ( 1-v ) );
        }
    private:
        float t;
        AGVector4 c0,c1;
};


Mesh* makeRingMesh ( AntMap *map ) {
    if ( !map->getScene() )
        return 0;
    Mesh *mesh=new ColoredMesh ( dynamic_cast<Scene*> ( map->getScene() ),getRingData(),AGVector4 ( 0,0,0,0 ),0 );
    mesh->setOrder ( RING_Z );
    return mesh;
}
Mesh* makeBigRingMesh ( AntMap *map ) {
    if ( !map->getScene() )
        return 0;
    Mesh *mesh=new ColoredMesh ( dynamic_cast<Scene*> ( map->getScene() ),getRingData ( 4 ),AGVector4 ( 0,0,0,0 ),0 );
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
