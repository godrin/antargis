#ifndef SCENE_BASE_H
#define SCENE_BASE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ant_camera.h"
#include "scenenode.h"


#include <set>

/**
   some helper structure, which is used for storing
   results when picking. it holds some information about:
   * distance to camera (for sorting)
   * the picked scene-node
   * and the 3d-position, where the scene-node was touched
   */
struct AGEXPORT PickNode
{
  AGVector4 pos;
  SceneNode *node;
  float camDist;
  
  bool operator<(const PickNode &n) const;
};

template<class T>
class QuadTree;


typedef std::vector<PickNode> PickResult;
typedef std::list<SceneNode*> SceneNodeList;

class AGEXPORT SceneBase
{
 public:
  //

  SceneBase(int w,int h);
  virtual ~SceneBase() throw();

//  void draw();

  // ATTENTION: nodes are not owned by Scene afterwards - so they won't get deleted!
  //            You have to do this yourself in the Entities or let ruby's GC do it for you (which would be the normal case)
  void addNode(SceneNode *node);
  void removeNode(SceneNode *node);
  void prepareUpdate(SceneNode *node);
  void updatePos(SceneNode *node);

  void clear() throw();

  // (mx,my,0)
  void setCamera(AGVector4 v);
  AGVector4 getCamera() const;
  virtual void advance(float time);

  /**
     picking is currently done with opengl. this uses software (at least on my box), which is
     pretty slow. Some new implementation using BSPs would be cool!
     VertexArray or MeshData should contain it's data in such a tree. rays can be transformed using
     inverse transformation-matrices. This way data can stay as is.
  */
  //  virtual PickResult pick(float x,float y,float w,float h);

  AntCamera &getCameraObject();

//  size_t getDrawnMeshes() const;

  /// width and height of screen
  float width() const;
  float height() const;



  /** get currently visible nodes */
  virtual SceneNodeList getCurrentNodes();

  //  void setEnabled(bool p);

 protected:

  typedef std::vector<SceneNode*> Nodes;
  typedef std::set<SceneNode*> NodeSet;

  typedef QuadTree<SceneNode> Tree;

  Tree *mTree;

  AntCamera mCamera;

  Nodes mNodes;
  NodeSet mNodeSet;

};


#endif
