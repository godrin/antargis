#include "ant_models.h"
#include <ag_xml.h>
#include <3dengine/mesh_data.h>
#include <anim_mesh_data.h>
#include <mesh.h>
#include <anim_mesh.h>
#include <entity.h>
#include <map.h>
#include <cmath>

struct StaticMeshDefinition {
    std::string meshfile,texture,animfile;
    bool culling;
    float scale;
    MeshData *data;
    AnimMeshData *animData;
};


typedef std::map<std::pair<AGString,AGString>,StaticMeshDefinition> StaticMeshes;

StaticMeshes staticMeshes;

StaticMeshes *loadStaticMeshes() {
    if ( staticMeshes.size() ==0 ) {
        Document doc;
        doc.parseMemory ( loadFile ( "data/models/smeshes.xml" ) );
        Node &root=doc.root();
        std::cout<<"ROOT:"<<root.getName() <<std::endl;
        for ( Node::iterator groupNode=root.begin();groupNode!=root.end();groupNode++ ) {
            AGString groupName= ( *groupNode )->get ( "name" );
            std::cout<<"NAME:"<<groupName<<"  "<< ( *groupNode )->getName() <<std::endl;
            Node::NodeVector sprites= ( *groupNode )->getChildren ( "sprite" );
            for ( Node::iterator sprite= sprites.begin();sprite!= sprites.end();sprite++ ) {
                StaticMeshDefinition smdef;
                AGString spriteName= ( *sprite )->get ( "name" );
                smdef.meshfile= ( *sprite )->get ( "mesh" );
                smdef.animfile= ( *sprite )->get ( "anim" );
                smdef.culling=std::string ( "true" ) == ( *sprite )->get ( "culling" );
                smdef.texture= ( *sprite )->get ( "texture" );
                smdef.scale= ( *sprite )->get ( "scale" ).toFloat();//FIXME
                smdef.data=0;
                smdef.animData=0;
                if (smdef.meshfile.length()>0) {

                    std::cout<<"LOADING:"<<smdef.meshfile<<" "<<smdef.scale<<" "<<smdef.texture<<"  "<< ( *sprite )->getName() <<std::endl;
                    MeshData *md=new MeshData ( smdef.meshfile,smdef.scale,smdef.texture );
                    std::cout<<"LOADED:"<<smdef.meshfile<<std::endl;

                    //if (true || !smdef.culling ) {
                    md->setCulling ( smdef.culling );
                    md->setTransparent(true);

                    smdef.data=md;
                } else if (smdef.animfile.length()>0) {
                    std::cout<<"LOADING animmesh:"<<smdef.animfile<<" to "<<groupName<<"/"<<spriteName<<std::endl;
                    AnimMeshData *md=new AnimMeshData(smdef.animfile);
                    smdef.animData=md;
                    md->setTransform(AGMatrix4(M_PI,
                                               AGVector3(0,0,1))*AGMatrix4(M_PI/2.0,AGVector3(1,0,0)));

                }
                staticMeshes[std::make_pair ( groupName,spriteName ) ]=smdef;
            }
        }
    }
    return &staticMeshes;
}

SceneNode *generateMesh ( AntEntity *entity ) {
    return 0; // NOT IMPLEMENTED BUSHs
}

SceneNode* AntModels::createModel ( Scene *scene, std::string type, std::string subtype, float angle ) {
    SceneNode *n;


    //n=generateMesh ( entity );
    //if ( n ) return n;

    StaticMeshes *m=loadStaticMeshes();
    std::cout<<"m-size:"<<m->size() <<std::endl;
    std::pair<AGString,AGString> ref=std::make_pair ( type,subtype );

    StaticMeshDefinition mdef=m->at(ref);
    MeshData* md=mdef.data;
    if (md) {
        std::cout<<"getting mesh for "<<type<<"/"<<subtype<<":"<<mdef.meshfile<<std::endl;
        return new Mesh ( scene,*md,AGVector4(),angle );
    } else {
        std::cout<<"getting mesh for "<<type<<"/"<<subtype<<":"<<mdef.animfile<<std::endl;
        AnimMeshData *amd=m->at(ref).animData;

        AnimMesh *m= new AnimMesh(scene,amd);
        return m;
    }
}
