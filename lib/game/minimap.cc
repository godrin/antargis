#include "minimap.h"
#include <ag_layoutfactory.h>
#include <ag_screen.h>
#include <ag_profiler.h>
#include "gl_app.h"
#include "entity.h"

#define MAP_BORDER

MiniMap::MiniMap(AGWidget *p,const AGRect2 &r,AntMap *pMap):
  AGWidget(p,r),
  sigMoved(this,"sigMoved"),
  mMap(pMap),
  mSurface(r.w(),r.h())
    {
      mMapBorder=24;
      mScene=0;
      mTexture=new AGTexture(mSurface);
      setMap(mMap);
    }

MiniMap::~MiniMap() throw()
  {
    checkedDelete(mTexture);
  }


bool MiniMap::mapChangedComplete(AGEvent *e)
  {
    mapChangedP(true);
    return false;
  }

bool MiniMap::mapChanged(AGEvent *e)
  {
    mapChangedP(false);
    return false;
  }

float MiniMap::getNoise(int x,int y)
  {
    return 1.0-((x*87644+y*653635+x*y*153637)%100)/float(1000.0);
  }

void MiniMap::mapChangedP(bool forceFull=false)
  {
    //  CTRACE;
    if(!mMap)
      return;
    int w,h;
    int x,y;
    //  forceFull=true;
    bool inmem=forceFull;

    w=getRect().w();
    h=getRect().h();

    AGRect2 change=mMap->getChangeRect();
    //  float mw=mMap->getW();
    //  float mh=mMap->getH();

    AGVector2 p0(change[0]),p1(change[1]);

    AGVector2 sp0(fromMapCoords(p0));
    AGVector2 sp1(fromMapCoords(p1));

    int x0=std::min(sp0[0],sp1[0]);
    int x1=std::max(sp0[0],sp1[0]);
    int y0=std::min(sp0[1],sp1[1]);
    int y1=std::max(sp0[1],sp1[1]);

    AGPainter *p=0;

    if((x1-x0)*(y1-y0)>w*h/8 || inmem)
      {
        TRACE;
        p=new AGPainter(mSurface);
        inmem=true;
        x0=0;
        y0=0;
        x1=width()-1;
        y1=height()-1;
      }
    else
      {
        p=new AGPainter(*mTexture);
        inmem=false;
      }



    AGVector3 light(-1,1,1);
    light.normalize();

    TerrainType t;

    assert(mMap);

    if(mMap)
      {
        CTRACE;
        for(x=x0;x<x1;x++)
          for(y=y0;y<y1;y++)
            {
              AGVector2 mv(toMapCoords(AGVector2(x,y)));

              AGColor c(0,0,0xFF);

              t=mMap->getTerrain(mv[0],mv[1]);
              float height=mMap->getHeight(mv[0],mv[1]);

              AGVector3 n=mMap->getNormalF(mv[0],mv[1]);

              float l=std::min(1.0f,n*light*0.5f+0.5f);

              if(t==WATER)
                c=AGColor(0xAA,0xAA,0xFF);
              else if(t==SAND)
                c=AGColor(0xFF,0x77,0);
              else if(t==EARTH)
                c=AGColor(0xAA,0x66,0);
              else if(t==GRASS)
                c=AGColor(0,0xaa,0);
              else if(t==GRASS2)
                c=AGColor(0x00,0xdd,0x00);
              else if(t==FOREST)
                c=AGColor(0x00,0x77,0x00);
              else if(t==ROCK)
                c=AGColor(0xAA,0xAA,0xAA);
              else if(t==ROCK2)
                c=AGColor(0xFF,0xFF,0xFF);

              c=c*l;

              if(height<=0)
                c=c*0.25+AGColor(0,0,0xFF)*0.75;
              else if(height<0.2)
                c=c*(height*4)+AGColor(0,0,0xFF)*(1-(height*4));
              else if(height<0.4)
                c=c*0.25+AGColor(0xFF,0x77,0)*0.75; // sand
              c*=getNoise(x,y);


              p->putPixel(AGVector2(x,y),c);
            }
      }
    else
      {
        for(x=0;x<w;x++)
          for(y=0;y<h;y++)
            {
              p->putPixel(AGVector2(x,y),AGColor(0xFF,0,0));
            }
      }

    checkedDelete(p);
    if(inmem)
      {
        //      mSurface.save("sicke.png");
        cdebug(mTexture);
        checkedDelete(mTexture);
        mTexture=new AGTexture(mSurface);
        cdebug(mTexture);
        CTRACE;
      }
    return;
  }

void MiniMap::draw(AGPainter &p)
  {
    STACKTRACE;
    //  CTRACE;
    AGRect2 m=getRect().origin();
    //  cdebug(mTexture<<"   "<<m);
    p.blit(*mTexture,m);

    drawEntities(p);

    if(mScene)
      {
        AGVector4 cam=mScene->getCamera();

        int w,h;

        AGVector2 v=fromMapCoords(cam.dim2());

        // now with approx. frustum

        w=25*getRect().w()/mMap->getW();
        h=25*getRect().h()/mMap->getH();

        v-=AGVector2(w/2,h/2);

        AGRect2 r(v[0],v[1],w,h);
        AGColor c1(0xff,0xaa,0,0x77);
        AGColor c2(0xee,0x77,0,0x77);

        p.drawBorder(r,2,c1,c2);
        r=r.shrink(1);
        p.drawBorder(r,2,c2,c1);
      }
  }

void MiniMap::drawEntities(AGPainter &p)
  {
    //  return;
    if(mMap)
      {
        std::vector<std::pair<AGRect2,AGVector4> > rects;
        size_t es=0;
        AntMap::EntityList e=mMap->getAllEntities();
        for(AntMap::EntityList::iterator i=e.begin();i!=e.end();i++)
          {
            if((*i)->showOnMinimap())
              {
                AGVector2 v=(*i)->getPos2D();
                v=fromMapCoords(v);

                float s=(*i)->getMinimapSize();
                AGVector4 c0=(*i)->getMinimapColor().toVec();
                AGVector4 c1=((*i)->getMinimapColor()*1.2).toVec();
                AGVector4 c2=((*i)->getMinimapColor()*1.5).toVec();

                if(s>2)
                  {
                    rects.push_back(std::make_pair(AGRect2(v[0],v[1],s,s),c2));
                    rects.push_back(std::make_pair(AGRect2(v[0]+1,v[1]+1,s-2,s-2),AGVector4(1,1,1,1)));
                    if(s>4)
                      rects.push_back(std::make_pair(AGRect2(v[0]+2,v[1]+2,s-4,s-4),c2));
                  }
                else
                  {
                    rects.push_back(std::make_pair(AGRect2(v[0],v[1],s,s),c0));
                    rects.push_back(std::make_pair(AGRect2(v[0],v[1],1,1),c2));
                    rects.push_back(std::make_pair(AGRect2(v[0]+1,v[1],1,1),c1));
                    rects.push_back(std::make_pair(AGRect2(v[0],v[1]+1,1,1),c1));
                  }
                es++;
              }
          }
        p.fillRects(rects); // batch processing - in gl much faster!!
      }
  }

void MiniMap::setMap(AntMap *pMap)
  {
    mMap=pMap;
    if(mMap)
      {
        mMap->sigMapChanged.connect(slot(this,&MiniMap::mapChanged));
        mMap->sigMapChangedComplete.connect(slot(this,&MiniMap::mapChangedComplete));
        mapChangedComplete(0);
      }
  }
void MiniMap::setScene(SceneBase *pScene)
  {
    mScene=pScene;
  }

bool MiniMap::eventMouseButtonDown(AGEvent *m)
  {
    if(mMap==0 || mScene==0)
      return AGWidget::eventMouseButtonDown(m);

    AGWidget::eventMouseButtonDown(m);

    // eat up event - so antView, doesn't get it
    return(hovered());
    /*      ret
    if(m->isSDLEvent())
      if(getRect().contains(m->getRelMousePosition()))
        return true;
    return false;*/
  }

bool MiniMap::eventMouseClick(AGEvent *m)
  {
    cdebug("clicked");
    AGVector2 p(m->getMousePosition()-getScreenRect()[0]);
    if(mMap==0 || mScene==0)
      return AGWidget::eventMouseClick(m);

    AGVector2 v(p);
    cdebug("clicked");

    float w=25*getRect().w()/mMap->getW();
    float h=25*getRect().h()/mMap->getH();

    w/=2;
    h/=2;
    if(v[0]<w)
      v[0]=w;
    if(v[1]<h)
      v[1]=h;
    if(v[0]>width()-w)
      v[0]=width()-w;
    if(v[1]>height()-h)
      v[1]=height()-h;

    v=toMapCoords(v);
    cdebug("sigMoved");
    mPos=v;
    sigMoved(m);
    /*
    //  mScene->setCamera(AGVector4(v[0],v[1],0,0));
    AGApplication *app=getApp();
    assert(app);
    if(app)
      {
        GLApp *glapp=dynamic_cast<GLApp*>(app);
        if(glapp)
          glapp->setCamera(v);
      }
     */
    return true;
  }

AGVector2 MiniMap::getMapPosition() const
{
  return mPos;
}


AGVector2 MiniMap::toMapCoords(AGVector2 v) const
{
  AGRect2 r=getRect();
  v[0]/=r.w();
  v[1]/=r.h();

  v[1]=1-v[1];

#ifdef MAP_BORDER
  v[0]*=mMap->getW()-mMapBorder*2;
  v[1]*=mMap->getH()-mMapBorder*1.5;
  v[0]+=mMapBorder;
  v[1]+=mMapBorder*0.35;
#else
  v[0]*=mMap->getW();
  v[1]*=mMap->getH();
#endif
  return v;
}
AGVector2 MiniMap::fromMapCoords(AGVector2 v) const
{
  AGRect2 r=getRect();

#ifdef MAP_BORDER
  v[0]-=mMapBorder;
  v[1]-=mMapBorder*0.35;

  v[0]/=mMap->getW()-mMapBorder*2;
  v[1]/=mMap->getH()-mMapBorder*1.5;
#else
  v[0]/=mMap->getW();
  v[1]/=mMap->getH();
#endif

  v[1]=1-v[1];

  v[0]*=r.w();
  v[1]*=r.h();

  return v;
}




MiniMap *toMiniMap(AGWidget *w)
  {
    return dynamic_cast<MiniMap*>(w);
  }

// AGLayout creator
class AGMiniMapLayoutCreator:public AGLayoutCreator
  {
    public:
      REGISTER_COMPONENT(MiniMap,"miniMap")

      virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode)
        {
          setResult(new MiniMap(pParent,pRect,0));
        }
  };

void registerMinimapCreator()
  {
    getLayoutFactory()->addCreator("miniMap",new AGMiniMapLayoutCreator);
  }

