#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scene.h"
#include <ag_geometry.h>
#include <ag_xml.h>
#include "entptr.h"
#include <ag_messageobject.h>
#include <ag_serial.h>

class AntEntity;
class AntEntityPtr;
class Scene;
class Mesh;
class TerrainBase;

enum TerrainType { WATER=0, SAND, EARTH, GRASS, GRASS2, FOREST, ROCK, ROCK2, LASTTERRAIN};

#define FIRSTTERRAIN WATER

class AGEXPORT HeightMap:public AGMessageObject
{
 public:
  HeightMap(SceneBase *pScene,int w,int h);
  virtual ~HeightMap() throw();

  // get status

  AGVector2 getNextPlaceAbove(const AGVector2 &p,float height) const;

  float getHeight(float x,float y) const;
  AGVector3 getNormal(int x,int y) const;
  AGVector3 getNormalF(float x,float y) const;
  AGVector4 getVertex(int x,int y);
  float get(size_t x,size_t y) const;
  //float getGrass(size_t x,size_t y) const;

  void setTerrain(size_t x,size_t y,TerrainType t,float v);
  float getTerrain(size_t x,size_t y,TerrainType t) const;

  float getMean(float x,float y);
  size_t getW() const
  {
    return mW;
  }
  size_t getH() const
  {
    return mH;
  }

  // truncate pos at borders
  AGVector2 truncPos(const AGVector2 &p) const;
  AGVector3 truncPos(const AGVector3 &p) const;

  // save load
  virtual void saveXML(Node &node) const;
  virtual bool loadXML(const Node &node);

  // editing
  void setHeight(float height); // for whole plane
  void set(size_t x,size_t y,float height);

  virtual void mapChanged();  


  void setTerrainScale(TerrainType t,float s);
  float getTerrainValue(float x,float y,TerrainType t);
  TerrainType getTerrain(float x,float y);
  float getTerrainWeight(float x,float y);
  float getTerrainScale(float x,float y);


  void addChange(const AGVector2 &v);

  AGRect2 getChangeRect() const;

  virtual void newMap(int w,int h);

  AGSignal sigMapChanged;
  AGSignal sigMapChangedComplete;

  SceneBase *getScene();

  /// override this function to include another terrain-mesh-type (like 2d-terrain)
  virtual void initTerrainMesh();

  /// to be used by initTerrainMesh() - not otherwise !!!
  void setTerrain(TerrainBase *pTerrain);
  TerrainBase *getTerrainMesh();

//  std::string hash() const;

 private:

  void checkTerrain();

  void loadBinary(BinaryIn &s);
  void saveBinary(BinaryOut &s) const;


  std::vector<float> mHeights;


  std::map<TerrainType,float> mTerrainScale;

  // sum of all should be one - or at least they get normalized to 1 and then the heighest is chosen and weighted with the mean value
  //std::map<TerrainType,std::vector<float> > mTerrainTypes;
  std::vector<std::vector<float> > mTerrainTypes;

  size_t mW,mH;

  AGRect2 mChangeRect;
  size_t mChanges;

  SceneBase *mScene;

 protected:

  TerrainBase *mTerrain;
  AGString mName;
};

#endif
