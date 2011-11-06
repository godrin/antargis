#ifndef __MESH_BASE_H
#define __MESH_BASE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scene.h"

#include <map>


/** Meshbase is a common base class for all types of meshes 
*/
class AGEXPORT MeshBase:public SceneNode
{
 public:
  MeshBase(SceneBase *s,const AGVector4 &pPos,const AGBox3 &pBox);
};

#endif


