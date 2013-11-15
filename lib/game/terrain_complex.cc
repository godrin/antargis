#include "terrain_complex.h"

int getTerrainDownScale();
int getTerrainDownScaleZ();
int getTerrainTriangleSize();

AntTerrainComplex::AntTerrainComplex(SceneBase *pScene,HeightMap &map):
  TerrainBase(pScene,map),
  m3D(getTextureCache()->get3D("data/textures/terrain/new3d.png",getTerrainDownScale(),getTerrainDownScaleZ())),
  mGrass(getTextureCache()->get("data/textures/terrain/grass4.png"))
  {
    init();

  }

void AntTerrainComplex::init()
  {
    size_t y,x;
    int tilesize=16;
    size_t tiles=0;

    HeightMap *map=getMap();

    for(y=0; y<map->getH();y+=tilesize)
      for(x=0;x<map->getW();x+=tilesize)
      {
        for(int i=0;i<0;i++) {
          TerrainPiece *t=new TerrainPiece(getScene(),this,*map,x,y,tilesize,tilesize,AGVector4(x,y,0,0),getTerrainTriangleSize(),-1);
          WaterPiece *w=new WaterPiece(getScene(),*map,x,y,tilesize,tilesize,AGVector4(x,y,0,0));
          pieces.push_front(t); // at least it's correct at the beginning
          water.push_front(w);
          mNodes.push_back(w);
          mNodes.push_back(t);
          tiles++;
        }
      }

    w=map->getW();
    h=map->getH();
  }

void AntTerrainComplex::mapChangedComplete()
{
  for(Pieces::iterator i=pieces.begin();i!=pieces.end();i++)
    checkedDelete(*i);
  for(WPieces::iterator i=water.begin();i!=water.end();i++)
    checkedDelete(*i);

  pieces.clear();
  water.clear();
  mNodes.clear();


  init();
}


AntTerrainComplex::~AntTerrainComplex() throw()
{
  for(Nodes::iterator i=mNodes.begin();i!=mNodes.end();i++)
    checkedDelete(*i);
}

void AntTerrainComplex::mapChanged()
{
  for(Pieces::iterator i=pieces.begin();i!=pieces.end();i++)
  {
    (*i)->mapChanged();
  }
  for(WPieces::iterator i=water.begin();i!=water.end();i++)
  {
    (*i)->mapChanged();
  }
}

AGTexture *AntTerrainComplex::get3dTexture()
{
  return &m3D;
}

AGTexture *AntTerrainComplex::getGrassTexture()
{
  return &mGrass;
}


