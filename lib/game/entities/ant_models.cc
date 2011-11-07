#include "ant_models.h"
#include <ag_xml.h>
#include <3dengine/mesh_data.h>
#include <mesh.h>
#include <entity.h>
#include <map.h>

struct StaticMeshDefinition {
    std::string meshfile,texture;
    bool culling;
    float scale;
    MeshData *data;
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
                smdef.culling=std::string ( "true" ) == ( *sprite )->get ( "culling" );
                smdef.texture= ( *sprite )->get ( "texture" );
                smdef.scale= ( *sprite )->get ( "scale" ).toFloat();//FIXME

                std::cout<<"LOADING:"<<smdef.meshfile<<" "<<smdef.scale<<" "<<smdef.texture<<"  "<< ( *sprite )->getName() <<std::endl;
                MeshData *md=new MeshData ( smdef.meshfile,smdef.scale,smdef.texture );
                std::cout<<"LOADED:"<<smdef.meshfile<<std::endl;

                //if (true || !smdef.culling ) {
                    md->setCulling ( smdef.culling );
                    md->setTransparent(true);
                
                smdef.data=md;
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

    MeshData* md= m->at ( ref ).data;
    return new Mesh ( scene,*md,AGVector4(),angle );
}
