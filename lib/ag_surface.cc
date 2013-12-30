/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_surface.cc
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

#include <iostream>

#include "ag_surface.h"
#include "ag_texture.h"
#include "ag_color.h"
#include "rk_debug.h"
#include "ag_vdebug.h"
#include "ag_draw.h"
#include "ag_sgeexport.h"
#include "ag_png.h"
#include "ag_glscreen.h"
#include "ag_sdlpainter.h"
#include "ag_surfacemanager.h"
#include "ag_surface_internal.h"

#include <SDL_image.h>

#include <sstream>
#include <cmath>

#include <ag_fs.h>

bool gDRM=false;


///////////////////////////////////////////////////////////////////////
// Tools
///////////////////////////////////////////////////////////////////////

bool gRendering=false;
void beginRender()
{
  gRendering=true;
}
void endRender()
{
  gRendering=false;
}

bool isRendering()
{
  return gRendering;
}


void AGFreeSurface(SDL_Surface *s)
{
  cdebug(s);
  SDL_FreeSurface(s);
}

AGInternalSurface *AGCreate32BitSurface(size_t width,size_t height)
{
  /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
     as expected by OpenGL for textures */
  AGInternalSurface *surface=new AGInternalSurface;
  Uint32 rmask, gmask, bmask, amask;

  /* SDL interprets each pixel as a 32-bit number, so our masks must depend
     on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else

  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif

  surface->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
      rmask, gmask, bmask, amask);
  if(!surface->surface)
  {
    std::cerr<<"CreateRGBSurface failed: "<<SDL_GetError()<<std::endl;
    //        throw UI::UIException();
  }

  return surface;
}





///////////////////////////////////////////////////////////////////////
// AGSurface
///////////////////////////////////////////////////////////////////////

AGSurface::AGSurface(AGInternalSurface *i):s(i)
{
  getSurfaceManager()->registerMe(this);
}


AGSurface::AGSurface():
  s(0)
  {
  getSurfaceManager()->registerMe(this);
  }
AGSurface::AGSurface(int w,int h):
  s(AGCreate32BitSurface(w,h))
  {
  getSurfaceManager()->registerMe(this);
  }
AGSurface::AGSurface(const AGSurface &p):
  s(p.s)
  {
  getSurfaceManager()->registerMe(this);
  }

bool AGSurface::valid() const
{
  return s && s->surface;
}

AGSurface AGSurface::copy() const
{
  AGInternalSurface *is=0;
  if(valid())
  {
    is=AGCreate32BitSurface(s->surface->w,s->surface->h);

    // copy pixel for pixel - as blitting doesn't seem to work
    for(int x=0;x<width();x++)
      for(int y=0;y<height();y++)
      {
        Uint32 c;
        Uint8 r,g,b,a;
        c=sge_GetPixel(s->surface,x,y);
        SDL_GetRGBA(c,s->surface->format,&r,&g,&b,&a);

        sge_PutPixel(is->surface,x,y,SDL_MapRGBA(is->surface->format,r,g,b,a));
      }
  }
  AGSurface ns;
  ns.s=is;
  return ns;
}

AGSurface::~AGSurface() throw()
{
  if(getSurfaceManager())
    getSurfaceManager()->deregisterMe(this);
}

void AGSurface::save(const std::string &pName) const
{
  if(gDRM==true)
    throw std::runtime_error("saving disabled - because of DRM!");
  assert(s);
  assert(s->surface);
  std::string png=toPNG(s->surface);
  saveFile(pName,png);
}


AGSurface &AGSurface::operator=(const AGSurface &p)
{
  s=p.s;
  return *this;
}

bool AGSurface::operator==(const AGSurface &p) const
{
  cdebug("operator==");
  if(width()!=p.width())
  {
    cdebug("width:"<<width()<<"!="<<p.width());
    return false;
  }
  if(height()!=p.height())
  {
    cdebug("height:"<<height()<<"!="<<p.height());
    return false;
  }
  for(int y=0;y<height();y++)
    for(int x=0;x<width();x++)
      if(getPixel(x,y)!=p.getPixel(x,y))
      {
        cdebug(x<<":"<<y<<":"<<getPixel(x,y)<<"!="<<p.getPixel(x,y));
        return false;
      }
  return true;
}



void AGSurface::putPixel(int x,int y,const AGColor &c)
{
  sge_PutPixel(s->surface,x,y,color(c));
  s->version++;
}

AGColor AGSurface::getPixel(int x,int y) const
{
  return AGColor(sge_GetPixel(s->surface,x,y),*this);
}



AGSurface AGSurface::getSubSurface(const AGRect2 &sr) const throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();

  AGInternalSurface *ns=AGCreate32BitSurface((int)sr.w(),(int)sr.h());

  AGRect2 dr(sr.origin());
  Uint32 c;
  Uint8 r,g,b,a;

  // copy
  for(int x=0;x<sr.w();x++)
    for(int y=0;y<sr.h();y++)
    {
      c=sge_GetPixel(s->surface,(int)(x+sr.x()),(int)(y+sr.y()));
      SDL_GetRGBA(c,s->surface->format,&r,&g,&b,&a);
      c=SDL_MapRGBA(ns->surface->format,r,g,b,a);

      sge_PutPixel(ns->surface,x,y,c);
    }
  AGSurface nas;
  nas.s=ns;
  return nas;
}


Uint32 AGSurface::color(const AGColor &c) const throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  return c.mapRGB(s->surface->format);
}

AGRect2 AGSurface::getRect() const throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  if(s)
    return AGRect2(0,0,s->surface->w,s->surface->h);
  else
    return AGRect2(0,0,0,0);
}

/**
 * @return width of the surface
 */
int AGSurface::width() const throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  return s->surface->w;
}
int AGSurface::height() const throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  return s->surface->h;
}

void AGSurface::drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)  throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  AGSDLPainter::drawGradientAlpha(s->surface,rect,ul,ur,dl,dr);
  s->version++;
}

void AGSurface::drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr) throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  AGSDLPainter::drawGradientAlpha(s->surface,rect,ul,ur,dl,dr);
  s->version++;
}
void AGSurface::drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2) throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();
  AGSDLPainter::drawBorder(s->surface,rect,W,c1,c2);
  s->version++;
}

void AGSurface::blit(const AGSurface &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor) throw(AGSurfaceEmptyException)
{
  if(!s)
    throw AGSurfaceEmptyException();

  int x,y,sx,sy,dx,dy;
  Uint32 c;
  Uint8 r,g,b,a;
  SDL_Surface *ss=const_cast<SDL_Surface*>(pSource.s->surface);
  SDL_Surface *ds=s->surface;

  Uint32 colorkey=ss->format->colorkey;
  int usekey=(ss->flags&SDL_SRCCOLORKEY);

  for(y=0,sy=(int)pSrc.y(), dy=(int)pDest.y();y<pSrc.h() ; y++,sy++,dy++)
    for(x=0,sx=(int)pSrc.x(), dx=(int)pDest.x();x<pSrc.w() ; x++,sx++,dx++)
    {
      c=sge_GetPixel(ss,sx,sy);

      if(usekey)
        if(c==colorkey)
          continue;

      SDL_GetRGBA(c,ss->format,&r,&g,&b,&a);
      c=SDL_MapRGBA(ds->format,r,g,b,a);

      sge_PutPixel(ds,dx,dy,c);

    }
  s->version++;
}

AGInternalSurface *AGSurface::surface() const
{
  return s;
}

AGSurface AGSurface::load(const std::string &pFilename) throw (FileNotFound)
{
  AGSurface n;
  n.s=new AGInternalSurface;
  std::string file=loadFile(pFilename);
  if(file.length()==0)
  {
    cdebug("File "<<pFilename<<" is empty!");
    throw FileNotFound(pFilename);
  }

  SDL_RWops* rw=SDL_RWFromMem(const_cast<char*>(file.c_str()),file.length());

  SDL_Surface *s=IMG_Load_RW(rw,0);
  if(!s)
  {
    std::cerr<<"error reading file:"<<pFilename<<std::endl;
  }


  SDL_FreeRW(rw);;

  n.s->surface=s;
  return n;
}


AGSurface AGSurface::scale(int w,int h) const
{
  AGSurface n(w,h);

  float ow=width();
  float oh=height();

  bool fast=false;

  for(int x=0;x<w;x++)
    for(int y=0;y<h;y++)
    {
      if(fast)
        n.putPixel(x,y,getPixel((int)(x*ow/w),(int)(y*oh/h)));
      else if(w>ow)
      {
        // interpolate
        float tx=x*ow/w;
        float ty=y*oh/h;

        int ix=(int)tx;
        int iy=(int)ty;

        float dx=tx-ix;
        float dy=ty-iy;

        n.putPixel(x,y,
            AGColor((getPixel(ix,iy  ).toVec()*(1-dx) + getPixel(ix+1,iy  ).toVec()*dx) * (1-dy)+
                (getPixel(ix,iy+1).toVec()*(1-dx) + getPixel(ix+1,iy+1).toVec()*dx) * dy     ));

      }
      else
      {
        // integrate
        size_t ps=0;
        float r,g,b,a;
        r=g=b=a=0;
        float tx0=x*ow/w;
        float ty0=y*oh/h;
        float tx1=(x+1)*ow/w;
        float ty1=(y+1)*oh/h;
        for(int mx=(int)tx0;mx<tx1;mx++)
          for(int my=(int)ty0;my<ty1;my++)
          {
            AGColor c=getPixel(mx,my);
            r+=c.r;
            g+=c.g;
            b+=c.b;
            a+=c.a;
            ps++;
          }
        if(ps>0)
        {
          r/=ps;
          g/=ps;
          b/=ps;
          a/=ps;
          n.putPixel(x,y,AGColor((int)r,(int)g,(int)b,(int)a));
        }


      }
    }

  return n;
}

AGSurface AGSurface::grayed() const
{
  AGSurface n(width(),height());

  for(int x=0;x<width();x++)
    for(int y=0;y<height();y++)
    {
      AGColor c=getPixel(x,y).grey();
      n.putPixel(x,y,c);
    }

  return n;
}

void AGSurface::fillRect(const AGRect2 &pr,const AGColor &c)
{
  for(int x=(int)pr.x0();x<=pr.x1();x++)
    for(int y=(int)pr.y0();y<=pr.y1();y++)
      putPixel(x,y,c);
}


void AGSurface::drawLine(const AGVector2 &pp0,const AGVector2 &pp1,const AGColor &c)
{
  AGVector2 p0,p1;

  p0=pp0;
  p1=pp1;

  float dx=p1[0]-p0[0];
  float dy=p1[1]-p0[1];

  if(fabs(dx)>fabs(dy))
  {
    if(dx<0)
    {
      p0=pp1;
      p1=pp0;
    }

    dx=p1[0]-p0[0];
    dy=p1[1]-p0[1];

    float y=p0[1];
    float e=0;
    float de=((float)dy)/dx;
    for(float x=p0[0];x<=p1[0];x++)
    {
      putPixel(int(x),int(y),c);
      e+=de;
      if(e>0.5)
      {
        e-=1.0;
        y++;
      }
      else if(e<-0.5)
      {
        e+=1.0;
        y--;
      }
    }
  }
  else
  {
    if(dy<0)
    {
      p0=pp1;
      p1=pp0;
    }

    dx=p1[0]-p0[0];
    dy=p1[1]-p0[1];

    float x=p0[0];
    float e=0;
    float de=((float)dx)/dy;
    for(float y=p0[1];y<=p1[1];y++)
    {
      putPixel(int(x),int(y),c);
      e+=de;
      if(e>0.5)
      {
        e-=1.0;
        x++;
      }
      else if(e<-0.5)
      {
        e+=1.0;
        x--;
      }
    }
  }
}

/**

@returns hot-spot int the left upper corner

 */
 AGVector2 AGSurface::shrink2Fit(int alphaThresh)
{
  int x0,x1;
  int y0,y1;
  int x,y;

  x0=width();
  y0=height();
  x1=0;
  y1=0;
  for(y=0;y<height();y++)
    for(x=0;x<width();x++)
    {
      if(getPixel(x,y).a>alphaThresh)
      {
        x0=std::min(x0,x);
        y0=std::min(y0,y);
        x1=std::max(x1,x);
        y1=std::max(y1,y);
      }
    }



  int nw=std::max(1,x1-x0+1);
  int nh=std::max(1,y1-y0+1);

  AGSurface n(nw,nh);

  for(x=0;x<nw;x++)
    for(y=0;y<nh;y++)
    {
      n.putPixel(x,y,getPixel(x+x0,y+y0));
    }
  (*this)=n;
  return AGVector2(x0,x1);

}

 /**
  * \return if this and pSurface have the same extends, this is sum_pixel((this(pixel)-pSurface(pixel)^2)
  *         otherwise it's -1
  *         the used norm for colors is sqrt((r0-r1)^2+(g0-g1)^2+(b0-b1)^2+(a0-a1)^2)
  * */
 float AGSurface::similarity(const AGSurface &pSurface) const
 {
   float value=0;
   if(pSurface.width()!=width() || pSurface.height()!=height())
   {
     return -1;
   }
   AGColor c0,c1;
   float r,g,b,a;
   for(int x=0;x<width();x++)
     for(int y=0;y<height();y++)
     {
       c0=getPixel(x,y);
       c1=pSurface.getPixel(x,y);
       r=c0.r-c1.r;
       b=c0.b-c1.b;
       g=c0.g-c1.g;
       a=c0.a-c1.a;
       value+=sqrt(r*r+b*b+g*g+a*a)/float(0xFF);
     }
   return value;
 }


 bool AGSurface::similarTo(const AGSurface &pSurface,float threshold) const
 {
   float s=similarity(pSurface);
   if(s<0)
     return s;
   if(width()==0 && height()==0)
     return true;
   return sqrt(s)/(width()*height())<threshold;
 }



 
 std::ostream &operator<<(std::ostream &o,SDL_PixelFormat *f)
 {
   o<<"(";
   //    SDL_Palette *palette;
   o<<(int)f->BitsPerPixel<<";";
   o<<(int)f->BytesPerPixel<<";";
   o<<(int)f->Rloss<<";";
   o<<(int)f->Gloss<<";";
   o<<(int)f->Bloss<<";";
   o<<(int)f->Aloss<<";";
   o<<(int)f->Rshift<<";";
   o<<(int)f->Gshift<<";";
   o<<(int)f->Bshift<<";";
   o<<(int)f->Ashift<<";";
   o<<(int)f->Rmask<<";";
   o<<(int)f->Gmask<<";";
   o<<(int)f->Bmask<<";";
   o<<(int)f->Amask<<";";
   o<<(int)f->colorkey<<";";
   o<<(int)f->alpha<<")";
   o<<"\n";


   return o;
 }

 std::ostream &operator<<(std::ostream &o,SDL_Surface *f)
 {
   o<<"(";
   o<<f->flags<<",";
   o<<f->format<<",";                /* Read-only */
   o<<f->w<<","<<f->h<<",";
   o<<f->pitch<<";";                           /* Read-only */
   o<<f->offset<<";";                             /* Private */

   o<<f->clip_rect<<";";                     /* Read-only */

   o<<f->locked<<";";                          /* Private */

   o<<f->format_version<<";";            /* Private */

   /* Reference count -- used when freeing surface */
   o<<f->refcount<<")\n";

   return o;
 }
