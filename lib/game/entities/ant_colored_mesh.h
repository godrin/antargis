#ifndef __ANT_COLORED_MESH_H
#define __ANT_COLORED_MESH_H

#include "mesh.h"

class ColoredMesh:public Mesh {
public:
    ColoredMesh ( Scene *scene,MeshData *data,AGVector4 pos,float angle );
    void setRingColor ( AGVector4 c );
    void advance ( float time );
    void draw();

private:
    float t;
    AGVector4 c0,c1;
};


#endif
