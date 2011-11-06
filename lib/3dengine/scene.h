#ifndef __SCENE_H
#define __SCENE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ag_geometry.h"

#include "ag_gl.h"

#include <vector>
#include <set>
#include <map>

#include "scenenode.h"
#include "scene_base.h"
#include "ant_camera.h"

/**
   \defgroup Engine3d 3d-engine of BoA
   \brief a very small 3d-engine used within BoA

   For more information go to \see Scene
*/





/** 
    \brief quad-tree based scene-manager
    \ingroup Engine3d

    Scene is scene-manager. it holds all the 3d-objects (meshes and particles).
    These objects are stored in a QuadTree (could easily be exchanged for an octree, if needed).
    It takes further for drawing modes like picking, shadow-rendering and depth-drawing (for shadow-depth-computation).
    It contains a camera-object!
    So in the current state it's not possible to render the same scene from to places at the same time.

 */

class AGEXPORT Scene:public SceneBase
{
 public:
  ////  typedef std::vector<PickNode> PickResult;
  ////  typedef std::list<SceneNode*> NodeList;

  Scene(int w,int h);
  virtual ~Scene() throw();

  void draw();

  // 0 == none, 1 == shadow mapping, 2 == perspective shadow mapping
  void setShadow(int v);
  int getShadow() const;


  // ATTENTION: nodes are not owned by Scene afterwards - so they won't get deleted!
  //            You have to do this yourself in the Entities or let ruby's GC do it for you (which would be the normal case)
  /*  void addNode(SceneNode *node);
  void removeNode(SceneNode *node);
  void prepareUpdate(SceneNode *node);
  void updatePos(SceneNode *node);
  

  void clear();

  // (mx,my,0)
  void setCamera(AGVector4 v);
  AGVector4 getCamera() const;*/
  virtual void advance(float time);
  
  /**
     picking is currently done with opengl. this uses software (at least on my box), which is
     pretty slow. Some new implementation using BSPs would be cool!
     VertexArray or MeshData should contain it's data in such a tree. rays can be transformed using
     inverse transformation-matrices. This way data can stay as is.
  */
  
  PickResult pick(float x,float y,float w,float h);
  /*
  AntCamera &getCameraObject();
*/
  size_t getDrawnMeshes() const;

  size_t getTriangles() const;
  size_t getPickTriangles() const;


  /**
     get 2d-Position on screen for a 3dim vector in 3-space
   */
  AGVector2 getPosition(const AGVector4 &v) const;

  /// get camera-viewing-direction to some 3d-point - used for particles
  AGVector3 getCameraDirTo(const AGVector3 &p) const;

  /*
  /// width and height of screen
  float width() const;
  float height() const;

  void mark();
  */
  AGMatrix4 getLightComplete() const;
  AGMatrix4 getLightView() const;
  AGMatrix4 getLightProj() const;

  /*
  AGVector2 getPosition(const AGVector4 &v) const;
  */
  SceneNodeList getCurrentNodes();


  void setEnabled(bool p);

 private:
  void calcShadowMap();
  void drawScene();
  void drawShadow();
  void initScene();

  void pickDraw();
  PickResult processHits (int hits, GLuint *buffer,float x,float y);

  Viewport getViewport() const;

  int mShadow;

  AGMatrix4 cameraPickMatrix;
  /*
  typedef std::vector<SceneNode*> Nodes;
  typedef std::set<SceneNode*> NodeSet;

  typedef QuadTree<SceneNode> Tree;

  Tree *mTree;

  AntCamera mCamera;

  Nodes mNodes;
  NodeSet mNodeSet;
  */
  AGVector4 white,black;

  size_t mTriangles;
  size_t mPickTriangles;
  size_t mMeshes;

  // picking vars
  std::map<GLuint,SceneNode*> pickNames;

  bool mEnabled;

  friend class AntImpostorData;
};


#endif
