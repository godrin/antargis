#ifndef MESH_2D_H
#define MESH_2D_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scenenode.h"
#include "scene_2d.h"
#include "mesh_base.h"
#include "mesh_2d_data.h"

class AGEXPORT Mesh2D:public MeshBase
{
 public:
  Mesh2D(Scene2D *pScene);
  Mesh2D(Scene2D *pScene,Mesh2DData &data,const AGVector4 &pPos,float pRot);
  virtual ~Mesh2D() throw();

  void draw();

  virtual AGVector4 lineHit(const AGLine3 &pLine) const;

  /// set rotation around y-axis (e.g. turn people)
  void setRotation(float r);
  /// set color (for rings)
  void setColor(const AGVector4 &pColor); 
  void setVisible(bool v);

  Mesh2DData *getData();

  bool hit(const AGVector2 &screenPos);

 public:

  AGRect2 getDrawingRect();
  AGRect2 getLastDrawingRect();

  Mesh2DData *mData;


 private:
  AGRect2 mLast;
};


#endif
