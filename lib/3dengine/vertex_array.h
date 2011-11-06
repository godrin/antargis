#ifndef __vertex_array_h
#define __vertex_array_h

#include "scene.h"
#include "glsl.h"
#include <ag_geometry.h>
#include <vector>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

#include <ag_surface.h>

/**
   VertexArray is for fast drawing meshes (3d-models).
   It supports indexed vertex-arrays. If you do not have them in this format
   look at mesh_optimizer.h.
   The possible drawing modes are detected automatically. The best one is always used.
   These include:
   * vertex-arrays
   * vertex-buffers
   * direct drawing mode (glVertex4f...)

   if you have to do special drawing like picking, use the corresp. functions (drawDepth,drawPick)
*/

class AGEXPORT VertexArray:public AGGLObject
{
  std::vector<AGVector4> mVertices,mColors;
  std::vector<AGVector3> mNormals;
  std::vector<AGVector2> mTexCoords;
  std::vector<AGVector3> mTexCoords3D;
  std::vector<Uint16>  mIndices;

  bool bColor;
  bool mBuffers;
  bool mArrays;
  bool mChanged;
  bool mTextures3D;

  /// VA is dynamic, when index-buffer changes
  bool mDynamic;

  unsigned int mVertexBuffer,mColorBuffer,mNormalBuffer,mTexBuffer,mIndexBuffer;

  AGBox3 bbox;

  GLuint displayList;
  bool displayListInited;

 public:
  VertexArray(bool pDynamic=false);
  virtual ~VertexArray();
  void addVertex(AGVector4 pVertex, AGVector4 pColor, AGVector3 pNormal, AGVector2 pTex);
  void addVertex(AGVector4 pVertex, AGVector4 pColor, AGVector3 pNormal, AGVector3 pTex);
  void addTriangle(size_t p0,size_t p1,size_t p2);

  void setColors(bool color);
  void setBuffers(bool pBuffers);
  
  virtual void draw();
  virtual void drawDepth();
  void drawPick();
  void init();

  void clear();
  void update();

  size_t getTriangles() const
  {
    return mIndices.size()/3;
  }
  
  size_t getIndex(size_t i);
  AGVector4 getVertex(size_t i);
  AGVector4 getColor(size_t i);
  AGVector3 getNormal(size_t i);
  AGVector2 getTexCoord(size_t i);

  void setTexCoord(size_t i,const AGVector2 &t);

  AGVector4 lineHit(const AGLine3 &pLine) const;

  bool useVertexArrays() const;
  friend VertexArray *makeInstances(const VertexArray &va,const std::vector<AGMatrix4> &ts);

  virtual void onScreenUp();
  virtual void onScreenDown();

};

/**
   makeInstances can be used for some kind of "batching".
   It creates several instances of a mesh stored in va. The positions of the instances are 
   given through transformation matrices in ts
*/
AGEXPORT VertexArray *makeInstances(const VertexArray &va,const std::vector<AGMatrix4> &ts);

/**
   VertexArrayShader adds shader-support to vertex-arrays. You might need to attach data
   to each vertex. You can do so by calling addAttribute(.,.).
 */
class AGEXPORT VertexArrayShader:public VertexArray
{
  AntShaderProgram *p;
  std::map<std::string,std::vector<float>*> as;
  std::map<std::string,unsigned int> aids;

  std::map<std::string,std::vector<Uint16>*> elementAs;
  std::map<std::string,unsigned int> elementIds;

  bool aInited;
 public:
  VertexArrayShader(AntShaderProgram *_p);
  ~VertexArrayShader();
  /// adds attributes to each vertex. the size of a should equal the vertex count
  void addAttribute(const std::string &pName,const std::vector<float> &a);
  void addAttribute(const std::string &pName,const std::vector<Uint16> &a);

  virtual void draw();

  virtual void onScreenUp();
  virtual void onScreenDown();

 private:
  void aInit();
  void attach();
  
};

#endif
