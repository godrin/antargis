/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_sdlsurface.cc
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#include "ag_sdlscreen.h"
#include "ag_texture.h"
#include "rk_debug.h"
#include "ag_fs.h"
#include "ag_draw.h"
#include "ag_fontengine.h"

#include "SDL_image.h"
#include "ag_sgeexport.h"
#include <cmath>

#include "ag_sdlpainter.h"

#include "ag_surface_internal.h"

static bool gUseSDLclipping=true;

SDL_Surface *AGCreate32BitSurface(size_t width,size_t height);

AGSDLScreen::AGSDLScreen(SDL_Surface *S):s(S)
{
  sge_Update_OFF();
}

void AGSDLScreen::flip()
  {
    CTRACE;
    SDL_Flip(s);
  }

void AGSDLScreen::update(const std::list<AGRect2> &rs)
  {
    CTRACE;
    SDL_Rect *nrs=new SDL_Rect[rs.size()];

    AGRect2 scr=getRect();
    size_t j=0;
    for(std::list<AGRect2>::const_iterator i=rs.begin();i!=rs.end();i++,j++)
      {
        AGRect2 n=scr.intersect(*i);
        nrs[j].x=(Sint16)n.x();
        nrs[j].y=(Sint16)n.y();
        nrs[j].w=(Uint16)n.w();
        nrs[j].h=(Uint16)n.h();
      }

    SDL_UpdateRects(s,rs.size(),nrs);

    checkedDeleteArray(nrs);
  }


AGRect2 AGSDLScreen::getRect() const
{
  return AGRect2(0,0,s->w,s->h);
}

void AGSDLScreen::fillRect(const AGRect2 &pRect,const AGColor &c)
  {
    if(c.a<0xFF)
      {
        sge_FilledRectAlpha(s,
            (int)pRect.x(),
            (int)pRect.y(),
            (int)(pRect.x()+pRect.w()-1),
            (int)(pRect.y()+pRect.h()-1),
            c.mapRGB(s->format),c.a);
        return;
        for(int x=(int)pRect.x0();x<(int)pRect.x1();x++)
          for(int y=(int)pRect.y0();y<(int)pRect.y1();y++)
            {
              putPixel(x,y,c);
            }
      }
    else
      {
        SDL_Rect r=pRect.sdl();
        SDL_FillRect(s,&r,c.mapRGB(s->format));
        return;
        for(int x=(int)pRect.x0();x<(int)pRect.x1();x++)
          for(int y=(int)pRect.y0();y<(int)pRect.y1();y++)
            putPixel(x,y,c);
      }
    return;
    sge_FilledRectAlpha(s,
        (int)pRect.x(),
        (int)pRect.y(),
        (int)(pRect.x()+pRect.w()-1),
        (int)(pRect.y()+pRect.h()-1),
        c.mapRGB(s->format),c.a);
  }

void AGSDLScreen::fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pRects)
  {
    for(std::vector<std::pair<AGRect2,AGVector4> >::const_iterator i=pRects.begin();i!=pRects.end();i++)
      {
        fillRect(i->first,i->second);
      }
  }

void AGSDLScreen::blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc)
  {
    SDL_Rect sr=pSrc.sdl();
    SDL_Rect dr=pDest.sdl();

    SDL_Surface *source=const_cast<AGTexture&>(pSource).sdlTexture()->surface;

    SDL_BlitSurface(source,&sr,s,&dr);
  }



AGScreen *mAGGScreen=0;
/*
bool videoInited()
{
  return mAGGScreen;
}*/

AGScreen & AGEXPORT getScreen()
  {
    assert(mAGGScreen);
    return *mAGGScreen;
  }


void setScreen(AGScreen *s)
  {
    TRACE;
    cdebug("s:"<<s);
    mAGGScreen=s;
  }


void AGSDLScreen::drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2)
  {
  }
void AGSDLScreen::putPixel(int x,int y,const AGColor &c)
  {
    sge_PutPixelAlpha(s,x,y,c.mapRGB(s->format),c.a);
  }


void AGSDLScreen::drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    AGSDLPainter::drawGradientAlpha(s,rect,ul,ur,dl,dr);

  }
void AGSDLScreen::drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    AGSDLPainter::drawGradient(s,rect,ul,ur,dl,dr);
  }

void AGSDLScreen::drawLine(const AGVector2 &pp0,const AGVector2 &pp1,const AGColor &c)
  {
    AGSDLPainter::drawLine(s,pp0,pp1,c);
  }

size_t AGSDLScreen::getWidth() const
{
  return s->w;
}
size_t AGSDLScreen::getHeight() const
{
  return s->h;
}

void AGSDLScreen::clip(const AGRect2 &r)
  {
    //  cdebug(r);
    if(gUseSDLclipping)
      {
        SDL_Rect sr=r.sdl();
        SDL_SetClipRect(s,&sr);
      }
  }
void AGSDLScreen::unclip()
  {
    if(gUseSDLclipping)
      {
        SDL_Rect sr=getRect().sdl();
        SDL_SetClipRect(s,&sr);
      }
  }

AGSurface AGSDLScreen::screenshotSurface(bool frontBuffer)
  {
    AGInternalSurface *surface=new AGInternalSurface;
    surface->surface=s;
    return AGSurface(surface).copy();
  }
AGTexture AGSDLScreen::screenshot(bool frontBuffer)
  {
    AGInternalSurface *surface=new AGInternalSurface;
    surface->surface=s;
    return AGTexture(AGSurface(surface).copy());
  }
