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
    float rotation;
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
        for ( Node::iterator groupNode=root.begin();groupNode!=root.end();groupNode++ ) {
            AGString groupName= ( *groupNode )->get ( "name" );
            Node::NodeVector sprites= ( *groupNode )->getChildren ( "sprite" );
            for ( Node::iterator sprite= sprites.begin();sprite!= sprites.end();sprite++ ) {
                StaticMeshDefinition smdef;
                AGString spriteName= ( *sprite )->get ( "name" );
                smdef.meshfile= ( *sprite )->get ( "mesh" );
                smdef.animfile= ( *sprite )->get ( "anim" );
                smdef.culling=std::string ( "true" ) == ( *sprite )->get ( "culling" );
                smdef.texture= ( *sprite )->get ( "texture" );
                smdef.scale= ( *sprite )->get ( "scale" ).toFloat();
                smdef.rotation= ( *sprite )->get ( "rotation" ).toFloat();
                smdef.data=0;
                smdef.animData=0;
                if (smdef.meshfile.length()>0) {

                    MeshData *md=new MeshData ( smdef.meshfile,smdef.scale,smdef.texture );
                    md->setCulling ( smdef.culling );
                    md->setTransparent(true);

                    smdef.data=md;
                } else if (smdef.animfile.length()>0) {
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
    std::pair<AGString,AGString> ref=std::make_pair ( type,subtype );

    StaticMeshes::iterator i=m->find(ref);
    if (i==m->end()) {
        std::cerr<<"Could not find mesh for "<<type<<"/"<<subtype<<std::endl;
        throw std::runtime_error(AGString("Could not find mesh for type:")+type+AGString(" and subtype:")+subtype);
    }

    StaticMeshDefinition mdef=i->second;
    MeshData* md=mdef.data;
    if (md) {
        Mesh *mesh=new Mesh ( scene,*md,AGVector4(),angle );
        mesh->setRotation(mdef.rotation);
        return mesh;
    } else {
        AnimMeshData *amd=m->at(ref).animData;

        AnimMesh *m= new AnimMesh(scene,amd);

        m->setAnimation(subtype);
        return m;
    }
}
