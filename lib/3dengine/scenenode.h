#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ant_renderer.h"
#include <ag_geometry.h>

class SceneBase;

/// these are the drawing orders, by which Scene::drawScene orders all the
/// meshes 1 will be drawn first and so forth

#define TERRAIN_Z 1
#define WATER_Z 2
#define DECAL_Z 3
#define RING_Z 4
#define MESH_Z 5
#define TREE_Z 6
#define PARTICLE_Z 7

/**
   A scenenode represents any part of a scene. It holds information
   about position, drawing order, bounding box and visibility.
   A scenenode is created for exactly one scene and it can't be
   assigned to any other scene, which shouldn't be necessary.

   Scene and SceneNode give each other information about their
   "destroy-state". That means the destructor notifies the other object.

*/
class AGEXPORT SceneNode {
public:
  SceneNode(SceneBase *s, const AGVector4 &pPos, const AGBox3 &pBox);
  virtual ~SceneNode() throw();

  /// reset my scene pointer - should not be called in "normal" code - only by
  /// the Scene-object
  virtual void resetScene();
  virtual void drawShadow();
  virtual void drawDepth();
  virtual void draw(Renderer *renderer);
  virtual void drawPick();

  /// advance mesh - e.g. animation
  virtual void advance(float time);

  virtual size_t getTriangles() const;
  virtual AGVector4 lineHit(const AGLine3 &pLine) const;

  /// sort triangles for given camera-view - shouldn't be used often!!!
  virtual void sort(const AGVector4 &pCamera);

  /// references equal??
  bool operator==(const SceneNode &n) const;

  virtual void setPos(const AGVector3 &pPos);
  AGVector4 getPos() const;
  virtual void setRotation(float r);
  void setBBox(const AGBox3 &pBox);

  /// indicates, if this object is transparent. if that's the case, it will be
  /// drawn later. drawing goes like this:1)draw all opaque objects from front
  /// to back; 2)draw transparent from back to front this is the fastest way to
  /// do this
  virtual bool transparent();

  /// get bounding box
  AGBox3 bbox() const;
  AGRect2 getRect() const;

  virtual void clear() throw();

  SceneBase *getScene();
  /// this checks, if the current object is inserted into a scene
  bool sceneValid() const;

  void setVisible(bool v);
  bool visible() const;

  /// there is a drawing order, used by the scene-manager, which priorized. look
  /// at Scene::drawScene for more info.
  void setOrder(int o);
  int getOrder() const;

  const AGString &getName() const;
  void setName(const AGString &pName);

private:
  void setScene(SceneBase *pScene);

  int mOrder;

  SceneBase *mScene;
  bool mVisible;

  AGVector4 mPos;
  AGBox3 mBBox;

  friend class SceneBase;
  AGString mName;
};

typedef SceneNode *SceneNodePtr;
#endif
