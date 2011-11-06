#ifndef __MESH_H
#define __MESH_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scene.h"
#include "mesh_base.h"
#include "vertex_array.h"

#include <map>

#include "mesh_data.h"

/** A mesh is a scenenode, that displays a mesh ;)
    ok, now for the real info.
    it contains a MeshData object, color and positional information.
    Actual drawing is done by MeshData.

*/
class AGEXPORT Mesh:public MeshBase
{
  MeshData *mData;
  float mRotation;
  AGVector4 mColor;
  bool mVisible;
  
 public:
  Mesh(Scene *pScene);
  Mesh(Scene *pScene,MeshData &data,const AGVector4 &pPos,float pRot);
  virtual ~Mesh()  throw();

  void draw();
  void drawDepth();
  void drawShadow();
  void drawPick();

  virtual AGVector4 lineHit(const AGLine3 &pLine) const;

  virtual size_t getTriangles() const;

  /// set rotation around y-axis (e.g. turn people)
  void setRotation(float r);
  /// set color (for rings)
  void setColor(const AGVector4 &pColor); 
  void setVisible(bool v);

  MeshData *getData();

  virtual bool transparent();


 private:
  void begin();
  void end();
};

#endif
