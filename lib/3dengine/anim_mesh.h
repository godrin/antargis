#ifndef ANIM_MESH_H
#define ANIM_MESH_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scene.h"
#include "mesh_base.h"
#include "ag_surface.h"
#include "ag_geometry.h"
#include "scenenode.h"

#include <vector>
#include <map>

#include "anim_mesh_data.h"

class AntEntity;

/**
   AnimMesh is a skeleton animated mesh.
   It support either shader-based and cpu-based animation.
 */
class AGEXPORT AnimMesh:public MeshBase
{
  AnimMeshData *mData;
  size_t curKey;
  float mTime;
  AGMatrix4 mTransform;
  AGMatrix4 mComplete;
  AGVector4 mRot;
  AGString mAnimName;
  Animation *mAnimation;
  AntEntity *mEntity;

  std::vector<AGMatrix4> mMatrices;
  std::vector<AGMatrix4> mShaderMatrices;

public:
  AnimMesh(Scene *pScene,AnimMeshData *data);
  virtual ~AnimMesh() throw();

  void setTransform(const AGMatrix4 &m);

  AGString getAnimation() const;
  bool setAnimation(const AGString &pName);

  void setPos(const AGVector3 &p);
  void setRotation(const AGVector3 &r,float a);
  void setRotation(float r);

  AnimMeshData *getData();

  virtual void draw();
  virtual void drawDepth();
  virtual void drawPick();
  virtual void advance(float time);


  void setEntity(AntEntity *e);

  virtual size_t getTriangles() const;

private:

  void drawPrivate(bool textured,bool mem);

  void update();
};

#endif
