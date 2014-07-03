#ifndef __antargis_gl_water_h
#define __antargis_gl_water_h

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "glsl.h"
#include "scene.h"
#include "ag_texturecache.h"
#include "ag_geometry.h"
#include "ag_texture.h"
#include "vertex_array.h"
#include "map.h"
#include <math.h>

class AntWaterShader;

/**
   A waterpiece is a scenenode, that displays some small patch of water. Water has
   currently always the same height (==0).
   The water is rendered with a VertexArray-object and is able to use some minor vertex shading
   for animation.
   In future some fragment shader using fresnel effects would be nice :-)
*/
class AGEXPORT WaterPiece:public SceneNode
{
  AGTexture tex;
 public:
  WaterPiece(SceneBase *pScene,HeightMap &map,int x,int y,int w,int h,const AGVector4 &pos);
  virtual ~WaterPiece() throw();

  /// draw in normal mode
  void draw();
  /// draw for picking (without shader)
  void drawPick();

  /// advance animation
  void advance(float t);
  size_t getTriangles() const;
  bool transparent();

  /// check for visibility, set bounding box and update vertex-array
  virtual void mapChanged();

 private:
  void addTriangle(int x0,int y0,int x1,int y1,int x2, int y2);

 private:
  VertexArray mArray;
  int mX;
  int mY;
  int mW;
  int mH;
  HeightMap *mMap;
  int step;
  AntWaterShader *mShader;
};
#endif

