#include "terrain.h"
#include <ag_texturecache.h>
#include <ag_rendercontext.h>
#include <ag_profiler.h>
#include <ag_config.h>
#include <ag_vdebug.h>

static bool gUse3dTextures=true;
void setUsing3dTextures(bool use3dTextures)
  {
    gUse3dTextures=use3dTextures;
  }

bool use3dTextures()
  {
    return gUse3dTextures;
  }

//////////////////////////////////////////////////////////////////////////
// TerrainPiece
//////////////////////////////////////////////////////////////////////////
TerrainPiece::TerrainPiece(SceneBase *pScene,TerrainBase *t,HeightMapInterface &map,int xs,int ys,int w,int h,const AGVector4 &pPos,int scale,int terrainType):
  SceneNode(pScene,AGVector4(),AGBox3()),
  mXs(xs),mYs(ys),mW(w),mH(h),
  mMap(&map),mTerrainType(terrainType)
  {
    mScale=scale;
    if(scale>0)
      {
        mXs/=scale;
        mYs/=scale;
        mW/=scale;
        mH/=scale;
      }
    mTerrain=t;
    mapChanged();
    setOrder(TERRAIN_Z);
  }

TerrainPiece::~TerrainPiece() throw()
  {
    if(sceneValid())
      getScene()->removeNode(this);
  }

void TerrainPiece::mapChanged()
  {
    AGBox3 bb=bbox();

    // first check, if this piece was really affected
    if(bb.valid())
      {
        AGRect2 r=AGRect2(AGVector2(bb.base[0]-2,bb.base[1]-2),AGVector2(bb.base[0]+bb.dir[0]+4,bb.base[1]+bb.dir[1]+4));  // add some border (2)

        if(!r.collide(mMap->getChangeRect()))
          {
            return;
          }
      }


    bb=AGBox3();
    m3dArray.clear();

    AGVector4 white(1,1,1,1);
    AGVector4 v;
    AGVector3 n;
    AGVector2 tp;
    AGVector3 tp3;
    float texFactor3w=0.5;

    size_t stepping=1;


    // add Vertices
    size_t x,y;
    for(x=mXs;x<=mXs+mW;x+=stepping)
      for(y=mYs;y<=mYs+mH;y+=stepping)
        {
          int sx=x*mScale,sy=y*mScale;
          v=mMap->getVertex(sx,sy);

          n=mMap->getNormal(sx,sy);

          if(use3dTextures())
            {
              float texHeight=mMap->getTerrainScale(sx,sy);

              tp3=AGVector3(-v[0]*texFactor3w,-v[1]*texFactor3w,texHeight);
              m3dArray.addVertex(v,white,n,tp3);
            }
          else
            {
              tp=AGVector2(-v[0]*texFactor3w,-v[1]*texFactor3w);
              m3dArray.addVertex(v,white,n,tp);
            }


          bb.include(v.dim3());
        }

    for(x=mXs;x<mXs+mW;x+=stepping)
      for(y=mYs;y<mYs+mH;y+=stepping)
        {
          int p0=(x-mXs)/stepping+(y-mYs)/stepping*(mW/stepping+1);
          int p1=p0+1;
          int p2=p0+(mW/stepping+1);
          int p3=p2+1;
          if(((x+y)%2)==0)
            {
              m3dArray.addTriangle(p2,p1,p0);
              m3dArray.addTriangle(p3,p1,p2);

            }
          else
            {
              m3dArray.addTriangle(p3,p1,p0);
              m3dArray.addTriangle(p3,p0,p2);
            }

        }
    setBBox(bb);
  }

void TerrainPiece::drawShadow()
  {
    STACKTRACE;
    m3dArray.setColors(false);
    m3dArray.draw();
    m3dArray.setColors(true);
  }
void TerrainPiece::drawDepth()
  {
    return;
    glDepthMask(true);
    m3dArray.setColors(false);
    m3dArray.draw();
    m3dArray.setColors(true);
  }

void TerrainPiece::drawPick()
  {
    STACKTRACE;
    //  cdebug(m3dArray.getTriangles());

    m3dArray.drawPick();
  }


void TerrainPiece::draw()
  {
    assertGL;

    STACKTRACE;
    AGRenderContext c;
    c.setLighting(true);
    if(use3dTextures())
      {
        c.setTexture(mTerrain->get3dTexture()->glTexture());
        mTerrain->get3dTexture()->setFilter(GL_LINEAR,GL_LINEAR);
      }
    else
      c.setTexture(mTerrain->getGrassTexture()->glTexture());

    c.begin();

    m3dArray.draw();
    assertGL;
  }


AGVector4 TerrainPiece::lineHit(const AGLine3 &pLine) const
{
  return m3dArray.lineHit(pLine);
}

size_t TerrainPiece::getTriangles() const
{
  return m3dArray.getTriangles();
}

int getTerrainTriangleSize()
  {
    int s=1;

    if(getConfig()->get("terrainTriangleSize")!="")
      s=getConfig()->get("terrainTriangleSize").toInt();

    getConfig()->set("terrainTriangleSize",AGString(s));

    return s;
  }


int getTerrainDownScale()
  {
    int s=1;

    if(getConfig()->get("terrainDownScaleExp")!="")
      s=getConfig()->get("terrainDownScaleExp").toInt();

    getConfig()->set("terrainDownScaleExp",AGString(s));

    return s;
  }

int getTerrainDownScaleZ()
  {
    int s=1;

    if(getConfig()->get("terrainDownScaleZExp")!="")
      s=getConfig()->get("terrainDownScaleZExp").toInt();

    getConfig()->set("terrainDownScaleZExp",AGString(s));

    return s;
  }

////////////////////////////////////////////////////////////////////////////
// TerrainBase
////////////////////////////////////////////////////////////////////////////


TerrainBase::TerrainBase(SceneBase *pScene,HeightMap &map):
  mMap(&map),mScene(pScene)
  {
    map.sigMapChanged.connect(slot(this,&TerrainBase::slotMapChanged));
    map.sigMapChangedComplete.connect(slot(this,&TerrainBase::slotMapChangedComplete));
  }

TerrainBase::~TerrainBase() throw()
  {
  }

bool TerrainBase::slotMapChanged(AGEvent *e)
  {
    mapChanged();
    return false;
  }
bool TerrainBase::slotMapChangedComplete(AGEvent *e)
  {
    mapChangedComplete();
    return false;
  }


SceneBase *TerrainBase::getScene()
  {
    return mScene;
  }

HeightMap *TerrainBase::getMap()
  {
    return mMap;
  }

void TerrainBase::mapChanged()
  {
  }

void TerrainBase::mapChangedComplete()
  {
  }


////////////////////////////////////////////////////////////////////////////
// Terrain
////////////////////////////////////////////////////////////////////////////

Terrain::Terrain(SceneBase *pScene,HeightMap &map):
  TerrainBase(pScene,map),
  m3D(getTextureCache()->get3D("data/textures/terrain/new3d.png",getTerrainDownScale(),getTerrainDownScaleZ())),
  mGrass(getTextureCache()->get("data/textures/terrain/grass4.png"))
  {
    init();

  }

void Terrain::init()
  {
    size_t y,x;
    int tilesize=16;
    size_t tiles=0;

    HeightMap *map=getMap();

    for(y=0; y<map->getH();y+=tilesize)
      for(x=0;x<map->getW();x+=tilesize)
        {
          TerrainPiece *t=new TerrainPiece(getScene(),this,*map,x,y,tilesize,tilesize,AGVector4(x,y,0,0),getTerrainTriangleSize(),-1);
          WaterPiece *w=new WaterPiece(getScene(),*map,x,y,tilesize,tilesize,AGVector4(x,y,0,0));
          pieces.push_front(t); // at least it's correct at the beginning
          water.push_front(w);
          mNodes.push_back(w);
          mNodes.push_back(t);
          tiles++;
        }

    w=map->getW();
    h=map->getH();
  }

void Terrain::mapChangedComplete()
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


Terrain::~Terrain() throw()
  {
    for(Nodes::iterator i=mNodes.begin();i!=mNodes.end();i++)
      checkedDelete(*i);
  }

void Terrain::mapChanged()
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

AGTexture *Terrain::get3dTexture()
  {
    return &m3D;
  }

AGTexture *Terrain::getGrassTexture()
  {
    return &mGrass;
  }

