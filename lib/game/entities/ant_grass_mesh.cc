#include "ant_grass_mesh.h"
#include <ag_rand.h>
#include <mesh.h>
#include <mesh_optimizer.h>
#include <cmath>

MeshData *getGrassMeshData ( float size,size_t many=4,const AGString & texture="",bool bush=false ) {

    size= ( ( int ) ( size*10 ) ) /10.0;
    static std::map<float,MeshData*> cache;




    if ( cache.find ( size ) !=cache.end() ) {
        return cache.at ( size );
    }

    MeshOptimizer opt;
    MeshVertex mv0,mv1,mv2,mv3;

    for ( size_t i=1;i<=many;i++ ) {

        mv0.c=mv1.c=mv2.c=mv3.c=AGVector4 ( 1,1,1,1 );

        if ( bush ) {
            mv0.t=AGVector2 ( 0,1-0 );
            mv1.t=AGVector2 ( 1,1-0 );
            mv2.t=AGVector2 ( 1,1-0.5 );
            mv3.t=AGVector2 ( 0,1-0.5 );
        } else {
            mv0.t=AGVector2 ( 0,1-0 );
            mv1.t=AGVector2 ( 1,1-0 );
            mv2.t=AGVector2 ( 1,1-1 );
            mv3.t=AGVector2 ( 0,1-1 );
        }

        float w=size;
        float h=w;

        if ( bush )
            h*=0.6;


        AGVector4 v0( -w,0,0 );
        AGVector4 v1( w,0,0 );
        AGVector4 v2( w,0,h*2 );
        AGVector4 v3( -w,0,h*2 );

        AGVector4 vadd;
        if ( i==1 )
            vadd=AGVector4 ( 0,0,0,0 );
        else
            vadd=AGVector4 ( agRand ( 1.0f ) *2,agRand ( 1.0f ) *2,0,0 );


        mv0.v=v0+vadd;
        mv1.v=v1+vadd;
        mv2.v=v2+vadd;
        mv3.v=v3+vadd;

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

        AGMatrix4 turn( M_PI/3,AGVector3 ( 0,0,1 ) );

        mv0.v=turn*v0+vadd;
        mv1.v=turn*v1+vadd;
        mv2.v=turn*v2+vadd;
        mv3.v=turn*v3+vadd;

        opt.add ( mv0 );
        opt.add ( mv1 );
        opt.add ( mv2 );

        opt.add ( mv0 );
        opt.add ( mv2 );
        opt.add ( mv3 );

        turn=AGMatrix4 ( -1*M_PI/3,AGVector3 ( 0,0,1 ) );

        mv0.v=turn*v0+vadd;
        mv1.v=turn*v1+vadd;
        mv2.v=turn*v2+vadd;
        mv3.v=turn*v3+vadd;

        opt.add ( mv0 );
        opt.add ( mv1 );
        opt.add ( mv2 );

        opt.add ( mv0 );
        opt.add ( mv2 );
        opt.add ( mv3 );

        if ( bush ) {
            mv0.t=AGVector2 ( 0,1-0.5 );
            mv1.t=AGVector2 ( 1,1-0.5 );
            mv2.t=AGVector2 ( 1,1-1 );
            mv3.t=AGVector2 ( 0,1-1 );
            mv0.n=AGVector3 ( 0,0,1 );
            mv1.n=AGVector3 ( 0,0,1 );
            mv2.n=AGVector3 ( 0,0,1 );
            mv3.n=AGVector3 ( 0,0,1 );
            mv0.v=AGVector4 ( -w,-w,h/3 );
            mv1.v=AGVector4 ( w,-w,h/3 );
            mv2.v=AGVector4 ( w, w,h/3 );
            mv3.v=AGVector4 ( -w, w,h/3 );
            opt.add ( mv0 );
            opt.add ( mv1 );
            opt.add ( mv2 );

            opt.add ( mv0 );
            opt.add ( mv2 );
            opt.add ( mv3 );
            mv0.v=AGVector4 ( -w*0.7,-w*0.7,h*0.7 );
            mv1.v=AGVector4 ( w*0.7,-w*0.7,h*0.7 );
            mv2.v=AGVector4 ( w*0.7, w*0.7,h*0.7 );
            mv3.v=AGVector4 ( -w*0.7, w*0.7,h*0.7 );
            opt.add ( mv0 );
            opt.add ( mv1 );
            opt.add ( mv2 );

            opt.add ( mv0 );
            opt.add ( mv2 );
            opt.add ( mv3 );

            mv0.v=AGVector4 ( -w/2,-w/2,h/2*3 );
            mv1.v=AGVector4 ( w/2,-w/2,h/2*3 );
            mv2.v=AGVector4 ( w/2, w/2,h/2*3 );
            mv3.v=AGVector4 ( -w/2, w/2,h/2*3 );
            opt.add ( mv0 );
            opt.add ( mv1 );
            opt.add ( mv2 );

            opt.add ( mv0 );
            opt.add ( mv2 );
            opt.add ( mv3 );
        }

    }


    cache[size]=new MeshData ( opt.getArray(),texture,true );
    cache[size]->setTransparent ( true );


    return cache[size];
}

Mesh* makeGrassMesh ( Scene *scene,float size) {
    if ( !scene )
        return 0;
    return new Mesh ( scene,*getGrassMeshData ( size,4,"data/textures/models/high_grass2.png" ),AGVector4 ( 0,0,0,0 ),0 );
}

Mesh *makeBushMesh ( Scene *scene,float size ) {
    if ( !scene )
        return 0;
    return new Mesh ( scene,*getGrassMeshData ( size,1,"data/textures/models/bush5.png",true ),AGVector4 ( 0,0,0,0 ),0 );
}
