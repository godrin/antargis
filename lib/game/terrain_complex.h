#ifndef __ANT_TERRAIN_COMPLEX_H
#define __ANT_TERRAIN_COMPLEX_H

#include "terrain.h"

class HeightMap;

class AGEXPORT AntTerrainComplex:public TerrainBase
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
  AntTerrainComplex(SceneBase *pScene,HeightMap &map);

  virtual ~AntTerrainComplex() throw();

  AGTexture *get3dTexture();
  AGTexture *getGrassTexture();

  /// some parts of the map are changed
  virtual void mapChanged();
  /// the whole map is changed - so better take care of this (texture-upload instead of repainting on GPU)
  virtual void mapChangedComplete();

  private:
  void init();
};

#endif

