#include "ant_colored_mesh.h"
#include <cmath>
#include <rk_debug.h>

ColoredMesh::ColoredMesh ( Scene *scene,MeshData *data,AGVector4 pos,float angle ) :
        Mesh ( scene,*data,pos,angle ) {
    if (!scene)
        throw int();
    t=0;
    c0=AGVector4 ( 0.6,0.6,0.6,1 );
    c1=AGVector4 ( 0.8,0.8,0.8,1 );
}
void ColoredMesh::setRingColor ( AGVector4 c ) {
    c0=c;
    c1=c*1.2;
    c1.setW ( 1 );
}
void ColoredMesh::advance ( float time ) {
    t+=time;
    float v= ( cos ( t*5 ) +1 ) /2;
    setColor ( c0*v+c1* ( 1-v ) );
}

void ColoredMesh::draw()
{
    Mesh::draw();
}
