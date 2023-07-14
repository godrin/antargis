#ifndef ANIM_MESH_H
#define ANIM_MESH_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ag_geometry.h"
#include "ag_surface.h"
#include "ant_renderer.h"
#include "mesh_base.h"
#include "scene.h"
#include "scenenode.h"

#include <map>
#include <vector>

#include "anim_mesh_data.h"

class AntEntity;

/**
   AnimMesh is a skeleton animated mesh.
   It support either shader-based and cpu-based animation.
 */
class AGEXPORT AnimMesh : public MeshBase {
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
  AnimMesh(Scene *pScene, AnimMeshData *data);
  virtual ~AnimMesh() throw();

  void setTransform(const AGMatrix4 &m);

  AGString getAnimation() const;
  bool hasAnimation(const AGString &pName) const;
  bool setAnimation(const AGString &pName);

  void setPos(const AGVector3 &p);
  void setRotation(const AGVector3 &r, float a);
  void setRotation(float r);

  AnimMeshData *getData();

  virtual void draw(Renderer *renderer);
  virtual void drawDepth(Renderer *renderer);
  virtual void drawPick(Renderer *renderer);
  virtual void advance(float time);

  void setEntity(AntEntity *e);

  virtual size_t getTriangles() const;

private:
  void drawPrivate(Renderer *renderer, bool textured, bool mem);

  void update();
};

#endif
