#include "ag_clip_painttarget.h"
#include "rk_debug.h"
#include "ag_projection.h"

AGClipPaintTarget::AGClipPaintTarget(AGPaintTarget *pTarget):mTarget(pTarget)
{
}

AGClipPaintTarget::~AGClipPaintTarget() throw()
  {}


void AGClipPaintTarget::blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest)
  {
    cdebug("NOT IMPLEMENTED!");
  }

void AGClipPaintTarget::blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor)
  {
    std::vector<std::pair<AGRect2,AGRect2> > l=mClipping.clip(pDest,pSrc);
    for(std::vector<std::pair<AGRect2,AGRect2> >::iterator i=l.begin();i!=l.end();i++)
      {
        mTarget->blit(pSource,i->first,i->second,pColor);
      }
  }

void AGClipPaintTarget::blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc)
  {
    std::vector<std::pair<AGRect2,AGRect2> > l=mClipping.clip(pDest,pSrc);
    for(std::vector<std::pair<AGRect2,AGRect2> >::iterator i=l.begin();i!=l.end();i++)
      {
        mTarget->blit(pSource,i->first,i->second);
      }
  }

void AGClipPaintTarget::blit(const AGSurface &pSource,const AGRect2 &pDest,const AGRect2 &pSrc)
  {
    std::vector<std::pair<AGRect2,AGRect2> > l=mClipping.clip(pDest,pSrc);
    for(std::vector<std::pair<AGRect2,AGRect2> >::iterator i=l.begin();i!=l.end();i++)
      {
        mTarget->blit(pSource,i->first,i->second);
      }
  }

void AGClipPaintTarget::drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c)
  {
    std::vector<AGLine2> l=mClipping.clip(AGLine2(p0,p1));
    for(std::vector<AGLine2>::iterator i=l.begin();i!=l.end();i++)
      mTarget->drawLine(i->getV0(),i->getV1(),c);
  }

void AGClipPaintTarget::fillRect(const AGRect2 &pr,const AGColor &c)
  {
    std::vector<AGRect2> l=mClipping.clip(pr);
    for(std::vector<AGRect2>::iterator i=l.begin();i!=l.end();i++)
      mTarget->fillRect(*i,c);
  }

void AGClipPaintTarget::fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pr)
  {
    std::vector<std::pair<AGRect2,AGVector4> > l;

    // collect clippings
    for(std::vector<std::pair<AGRect2,AGVector4> >::const_iterator i=pr.begin();i!=pr.end();i++)
      {
        std::vector<AGRect2> l2=mClipping.clip(i->first);
        for(std::vector<AGRect2>::iterator i2=l2.begin();i2!=l2.end();i2++)
          l.push_back(std::make_pair(*i2,i->second));
      }
    mTarget->fillRects(l);
  }

void AGClipPaintTarget::blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor)
  {
    std::vector<std::pair<AGRect2,AGRect2> > l;

    // collect clippings
    for(std::vector<std::pair<AGRect2,AGRect2> >::const_iterator i=pSrcDestRects.begin();i!=pSrcDestRects.end();i++)
      {
        std::vector<std::pair<AGRect2,AGRect2> > l2=mClipping.clip(i->first,i->second);
        std::copy(l2.begin(),l2.end(),std::back_inserter(l));
      }
    mTarget->blit(pSource,l,pColor);
  }

void AGClipPaintTarget::tile(const AGTexture &pSource,const AGRect2 &pTarget, const AGColor &pColor)
  {
    //std::vector<std::pair<AGRect2,AGRect2> > l2=mClipping.clip(i->first);

    cdebug("WARNING: AGClipPaintTarget::tile not implemented yet!");
  }

void AGClipPaintTarget::putPixel(int x,int y,const AGColor &c)
  {
    if(mClipping.included(AGVector2(x,y)))
      mTarget->putPixel(x,y,c);
  }

AGColor AGClipPaintTarget::getPixel(int x,int y) const
{
  return mTarget->getPixel(x,y);
}

// manage painting sessions
void AGClipPaintTarget::clip(const AGClipping &c)
  {
    mClipping=c;
  }

void AGClipPaintTarget::unclip()
  {
    mClipping=AGClipping();
  }

AGRect2 AGClipPaintTarget::getRect() const
{
  return mTarget->getRect();
}

void AGClipPaintTarget::drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    CTRACE;
    std::vector<AGRect2> rs=mClipping.clip(rect);

    AGRect2 t(0,0,1,1);
    AGProjection2D p(rect,t);

    for(std::vector<AGRect2>::iterator i=rs.begin();i!=rs.end();i++)
      {
        AGRect2 r2=p.project(*i);
        float x,y;
        AGColor ul2,ur2,dl2,dr2;

        x=r2.x0();y=r2.y0();
        ul2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x1();y=r2.y0();
        ur2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x0();y=r2.y1();
        dl2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x1();y=r2.y1();
        dr2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        mTarget->drawGradientAlpha(*i,ul2,ur2,dl2,dr2);

      }
  }

void AGClipPaintTarget::drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    std::vector<AGRect2> rs=mClipping.clip(rect);

    AGRect2 t(0,0,1,1);
    AGProjection2D p(rect,t);

    for(std::vector<AGRect2>::iterator i=rs.begin();i!=rs.end();i++)
      {
        AGRect2 r2=p.project(*i);
        float x,y;
        AGColor ul2,ur2,dl2,dr2;

        cdebug("rect:"<<rect);
        cdebug("r2:"<<r2);
        cdebug("i:"<<*i);

        x=r2.x0();y=r2.y0();
        ul2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x1();y=r2.y0();
        ur2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x0();y=r2.y1();
        dl2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        x=r2.x1();y=r2.y1();
        dr2=(ul*(1-x)+ur*x)*(1-y)+
        (dl*(1-x)+dr*x)*y;

        cdebug("ul:"<<ul);
        cdebug("ur:"<<ur);
        cdebug("dl:"<<dl);
        cdebug("dr:"<<dr);

        cdebug("ul2:"<<ul2);
        cdebug("ur2:"<<ur2);
        cdebug("dl2:"<<dl2);
        cdebug("dr2:"<<dr2);

        mTarget->drawGradient(*i,ul2,ur2,dl2,dr2);

      }
  }
