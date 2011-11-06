#ifndef BOA_3D_WIREFRAME_H
#define BOA_3D_WIREFRAME_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <scenenode.h>
#include "vertex_array.h"

class AGEXPORT Boa3dWireframe:public SceneNode
{
  struct Line
  {
    AGVector3 a,b;
  };

 public:
  Boa3dWireframe(Scene *pScene,const AGVector4 &pColor,float pWidth);
  virtual ~Boa3dWireframe() throw();

  void draw();

  void addLine(const AGVector3 &a,const AGVector3 &b);

  bool transparent();

 private:
  std::vector<Line> mLines;
  AGVector4 mColor;
  VertexArray mArray;

  bool mChanged;

  float mWidth;

};


#endif

