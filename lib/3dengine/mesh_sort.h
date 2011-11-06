#ifndef MESH_SORT_H
#define MESH_SORT_H

#include <ag_geometry.h>
#include "scenenode.h"

/**
   The class is for sorting purpose only. It provides the operator() function, that's needed
   for the STL-sorting algorithms.
   Here SceneNodes are sorted by their middle distance the camera.
*/
class SortDistance
{
  AGVector3 cam;
public:
  SortDistance(AGVector3 c):cam(c){}

  bool operator()(const SceneNode *n1,const SceneNode *n2);
};

/**
   This is a sorting class, too. It sorts by the given "SortOrder" of the scene-nodes
*/
class SortOrder
{
public:
  SortOrder(){}

  bool operator()(const SceneNode *n1,const SceneNode *n2);
};

class SortYCoord
{
 public:
  SortYCoord(){}
  
  bool operator()(const SceneNode *n1,const SceneNode *n2);
};


#endif
