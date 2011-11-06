#ifndef __antargis_gl_terrain_h
#define __antargis_gl_terrain_h

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <vector>

#include "scene.h"
#include "ag_geometry.h"
#include <ag_surface.h>
#include <math.h>

#include "vertex_array.h"
#include "water.h"

#include <ag_xml.h>

#include "map.h"

using namespace std;

class Terrain;

/**
   TerrainPiece displays (like WaterPiece) some small patch of terrain.
   (Terrain and water is tiled for the ease of culling)
   for more information on drawing-modes and such go to SceneNode.
 */
class AGEXPORT TerrainPiece:public SceneNode
{
 public:
  TerrainPiece(SceneBase *pScene,Terrain *t,HeightMap &map,int x,int y,int w,int h,const AGVector4 &pPos,int scale);
  virtual ~TerrainPiece() throw();

  void draw();
  void drawShadow();
  void drawDepth();
  void drawPick();

  // returns (0,0,0,0) if no collision
  // returns (x,y,z,1) if collision takes place
  AGVector4 lineHit(const AGLine3 &pLine) const;
  size_t getTriangles() const;

  /// rebuilds the mesh
  virtual void mapChanged();
  
 private:

  Terrain *mTerrain;

  size_t mXs,mYs;
  size_t mW,mH;
  int mScale;
  HeightMap *mMap;

  VertexArray m3dArray;
};


class AGEXPORT TerrainBase
{
 public:
  TerrainBase(SceneBase *pScene,HeightMap &map);
  virtual ~TerrainBase() throw();

  HeightMap *getMap();
  SceneBase *getScene();

  /// some parts of the map are changed
  virtual void mapChanged();
  /// the whole map is changed - so better take care of this (texture-upload instead of repainting on GPU)
  virtual void mapChangedComplete();


  virtual bool slotMapChanged(AGEvent *e);
  virtual bool slotMapChangedComplete(AGEvent *e);


 private:
  /// the height-map
  HeightMap *mMap;
  
  SceneBase *mScene;
};

/**
   In a game-world there exists exactly one Terrain-object.
   This object holds references to all water- and terrain-pieces, so
   that in case the terrain changes these will be changed,too.

*/
class AGEXPORT Terrain:public TerrainBase
{
  // Some containers for all the meshes
  typedef std::list<TerrainPiece*> Pieces;
  typedef std::list<WaterPiece*> WPieces;
  typedef std::list<SceneNode*> Nodes;
  Nodes mNodes;
  Pieces pieces;
  WPieces water;

  float w,h;

  /// 3d-texture used for the terrain
  AGTexture m3D;

  /// some (currently not used) grass-texture
  AGTexture mGrass;


public:
  Terrain(SceneBase *pScene,HeightMap &map);

  virtual ~Terrain() throw();

  AGTexture *get3dTexture();
  AGTexture *getGrassTexture();

  /// some parts of the map are changed
  virtual void mapChanged();
  /// the whole map is changed - so better take care of this (texture-upload instead of repainting on GPU)
  virtual void mapChangedComplete();

 private:
  void init();
};

AGEXPORT void setUsing3dTextures(bool use3dTextures);


#endif
