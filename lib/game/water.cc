#include "water.h"
#include "ag_vdebug.h"
#include "ag_kill.h"
#include "ag_rendercontext.h"
#include "ag_profiler.h"
#include "ag_config.h"

AntWaterShader *gWaterShader=0;
AntWaterShader *getWaterShader()
  {
    if(!gWaterShader)
      {
        gWaterShader=new AntWaterShader;
        REGISTER_SINGLETON(gWaterShader);
      }
    assert(gWaterShader);
    return gWaterShader;
  }

static int gWaterAnim=-10;
bool useWaterAnimation()
  {
    if(gWaterAnim==-10)
      {
        gWaterAnim=getConfig()->get("animationType")!="false";
      }
    return gWaterAnim;
  }


WaterPiece::WaterPiece(SceneBase *pScene,HeightMap &map,int x,int y,int w,int h,const AGVector4 &pos):
  SceneNode(pScene,pos,AGBox3()),
  mX(x),mY(y),mW(w),mH(h),mMap(&map)
  {
    CTRACE;

    step=2;
    tex=getTextureCache()->get("data/textures/terrain/water.png");

    //  mShader=new AntWaterShader;

    mapChanged();
    setOrder(WATER_Z);
  }


WaterPiece::~WaterPiece() throw()
  {
    if(sceneValid())
      getScene()->removeNode(this);

  }

void WaterPiece::mapChanged()
  {
    AGBox3 bb;
    mArray.clear();

    setVisible(false);
    bool vis=false;
    int x,y;
    int step=2;
    for(x=0;x<mW+step;x+=step)
      for(y=0;y<mH+step;y+=step)
        {
          AGVector4 p(x,y,0);
          p+=getPos();
          AGVector4 c(1,1,1,0.7);
          AGVector3 n(0,0,1);
          AGVector2 t(x,y);

          mArray.addVertex(p,c,n,t);

          bb.include(p.dim3()-AGVector3(0,0,1));
          bb.include(p.dim3()+AGVector3(0,0,1));

          if(mMap->get(mX+x,mY+y)<0.1 && !vis)
            {
              vis=true;
            }
        }
    setBBox(bb-getPos().dim3());

    for(x=0;x<mW/step;x++)
      for(y=0;y<mH/step;y++)
        {
          addTriangle(x,y, x+1,y, x,y+1);

          addTriangle(x+1,y, x+1,y+1, x,y+1);
        }

    if(vis)
      setVisible(true);
  }

void WaterPiece::addTriangle(int x0,int y0,int x1,int y1,int x2, int y2)
  {
    int w=(mW/step)+1;
    mArray.addTriangle(x2+y2*w,x1+y1*w,x0+y0*w);
  }

void WaterPiece::draw()
  {
    STACKTRACE;
    assertGL;

    AGRenderContext c;
    c.setAlpha(0,GL_NONE);
    c.setCulling(false);
    c.setTexture(tex.glTexture());
    //  tex.setClamp(GL_REPEAT,GL_REPEAT,GL_REPEAT);//CLAMP_TO_EDGE);
    //  c.setColor(AGVector4(0,0,1,1));
    c.begin();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glColor4f(1,1,1,1);
    if(useWaterAnimation())
      getWaterShader()->enable();
    mArray.draw();
    if(useWaterAnimation())
      getWaterShader()->disable();
    assertGL;
  }

void WaterPiece::drawPick()
  {
    STACKTRACE;
    //cdebug(mArray.getTriangles());
    //  mArray.drawPick();
  }

void WaterPiece::advance(float t)
  {
    assertGL;
    getWaterShader()->update(t);
    assertGL;
  }

size_t WaterPiece::getTriangles() const
{
  return mArray.getTriangles();
}

bool WaterPiece::transparent()
  {
    return true;
  }

