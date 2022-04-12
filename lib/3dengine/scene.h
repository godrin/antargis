#ifndef __SCENE_H
#define __SCENE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ag_geometry.h"

#include "ag_gl.h"

#include <map>
#include <set>
#include <vector>

#include "ant_camera.h"
#include "scene_base.h"
#include "scenenode.h"

/**
   \defgroup Engine3d 3d-engine of BoA
   \brief a very small 3d-engine used within BoA

   For more information go to \see Scene
*/

/**
    \brief quad-tree based scene-manager
    \ingroup Engine3d

    Scene is scene-manager. it holds all the 3d-objects (meshes and particles).
    These objects are stored in a QuadTree (could easily be exchanged for an
   octree, if needed). It takes further for drawing modes like picking,
   shadow-rendering and depth-drawing (for shadow-depth-computation). It
   contains a camera-object! So in the current state it's not possible to render
   the same scene from to places at the same time.

 */

class AGEXPORT Scene : public SceneBase {
public:
  Scene(int w, int h);
  virtual ~Scene() throw();

  void draw();

  // 0 == none, 1 == shadow mapping, 2 == perspective shadow mapping
  void setShadow(int v);
  int getShadow() const;

  virtual void advance(float time);

  /**
     picking is currently done with opengl. this uses software (at least on my
     box), which is pretty slow. Some new implementation using BSPs would be
     cool! VertexArray or MeshData should contain it's data in such a tree. rays
     can be transformed using inverse transformation-matrices. This way data can
     stay as is.
  */

  PickResult pick(float x, float y, float w, float h);
  size_t getDrawnMeshes() const;

  size_t getTriangles() const;
  size_t getPickTriangles() const;

  /**
     get 2d-Position on screen for a 3dim vector in 3-space
   */
  AGVector2 getPosition(const AGVector4 &v) const;

  /// get camera-viewing-direction to some 3d-point - used for particles
  AGVector3 getCameraDirTo(const AGVector3 &p) const;

  AGMatrix4 getLightComplete() const;
  AGMatrix4 getLightView() const;
  AGMatrix4 getLightProj() const;

  SceneNodeList getCurrentNodes();

  void setEnabled(bool p);

private:
  void calcShadowMap();
  void drawScene();
  void drawShadow();
  void initScene();

  void pickDraw();
  PickResult processHits(int hits, GLuint *buffer, float x, float y);

  Viewport getViewport() const;

  int mShadow;

  AGMatrix4 cameraPickMatrix;
  AGVector4 white, black;

  size_t mTriangles;
  size_t mPickTriangles;
  size_t mMeshes;

  // picking vars
  std::map<GLuint, SceneNode *> pickNames;

  bool mEnabled;

  friend class AntImpostorData;
};

#endif
