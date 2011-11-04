/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_texture.cc
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

#include "ag_texture.h"
#include "ag_glscreen.h"
#include "rk_debug.h"
#include "ag_rendercontext.h"
#include "ag_sgeexport.h"
#include "ag_surfacemanager.h"
#include "ag_profiler.h"
#include "ag_glpainter.h"
#include "ag_fbo.h"
#include "ag_main.h"
#include "ag_sdlpainter.h"
#include "ag_video.h"

#include "ag_surface_internal.h"

#include <stdexcept>

size_t nextpow2(size_t i)
  {
    size_t j=1;
    while(j<i)
      j<<=1;

    return j;
  }


///////////////////////////////////////////////////////////
// AGTexture
///////////////////////////////////////////////////////////


AGTexture::AGTexture(const AGTexture &t)
  {
    mSDLTexture=t.mSDLTexture;
    mTexture=t.mTexture;

    mTextureUsed=false;
    mPainting=false;
    m3d=t.m3d;
    s=t.s;

    w=t.w;
    h=t.h;
    version=t.version;
    getSurfaceManager()->registerMe(this);
    mCleared=false;

    mFBO=0;
  }

AGTexture &AGTexture::operator=(const AGTexture &t)
  {
    mSDLTexture=t.mSDLTexture;
    mTexture=t.mTexture;

    mTextureUsed=false;
    mPainting=false;
    m3d=t.m3d;
    s=t.s;

    w=t.w;
    h=t.h;
    version=t.version;
    getSurfaceManager()->registerMe(this);
    mCleared=false;
    mFBO=0;
    return *this;
  }


AGTexture::AGTexture()
  {
    version=0;
    w=h=0;
    s=0;
    m3d=false;
    mTexture=0;
    mSDLTexture=0;
    mTextureUsed=false;
    getSurfaceManager()->registerMe(this);
    mPainting=false;
    mCleared=false;
    mFBO=0;
  }

AGTexture::AGTexture(int W,int H):w(W),h(H),s(0)
  {
    m3d=0;
    version=0;
    mTexture=0;
    mSDLTexture=0;
    mTextureUsed=false;
    mPainting=false;
    getSurfaceManager()->registerMe(this);
    mCleared=true;
    mFBO=0;
  }



AGTexture::AGTexture(const AGSurface &pSurface, bool p3d)
  {
    version=0;
    w=pSurface.width();
    h=pSurface.height();
    m3d=p3d;

    mTextureUsed=false;
    mTexture=0;
    mSDLTexture=0;

    s=pSurface.surface();

    getSurfaceManager()->registerMe(this);
    mPainting=false;
    mCleared=false;
    mFBO=0;
  }


AGTexture::~AGTexture() throw()
  {
    if(getSurfaceManager())
      getSurfaceManager()->deregisterMe(this);
    checkedDelete(mFBO);
  }

int AGTexture::width() const
{
  return w;
}
int AGTexture::height() const
{
  return h;
}

AGRect2 AGTexture::getRect() const
{
  return AGRect2(0,0,w,h);
}

AGColor AGTexture::getPixel(int x,int y) const
{
  assert(!opengl());
  Uint32 c=sge_GetPixel(s->surface,x,y);
  Uint8 r,g,b,a;
  SDL_GetRGBA(c,s->surface->format,&r,&g,&b,&a);
  return AGColor(r,g,b,a);
}

bool AGTexture::hasTexture() const
{
  return mTexture || !opengl();
}
void AGTexture::clearTexture()
  {
    mTexture=0;
    checkedDelete(mFBO);
    mFBO=0;
  }
void AGTexture::clearTextureUsed()
  {
    mTextureUsed=false;
  }

AGInternalSurface *AGTexture::sdlTexture()
  {
    assert(!opengl());
    if(mSDLTexture==0)
      {
        mSDLTexture=new AGInternalSurface;
        assert(s);
        bool sempty=false;
        if(!s)
          {
            s=AGSurface(w,h).surface();
            sempty=true;
          }

        mSDLTexture->surface=SDL_DisplayFormatAlpha(s->surface);

        s->sdlTexture=mSDLTexture;
      }
    return mSDLTexture;
  }


AGGLTexture *AGTexture::glTexture()
  {
    assert(opengl());
    if(mTexture==0)
      {
        if(s)
          if(s->glTexture && version==s->version)
            mTexture=s->glTexture;
        cdebug(mTexture);
        if(!mTexture)
          {
            cdebug(m3d);
            if(m3d)
              mTexture=new AGGLTexture(nextpow2(std::min(w,h)),nextpow2(std::min(w,h)),nextpow2(std::max(w,h)/std::min(w,h)));
            else
              mTexture=new AGGLTexture(nextpow2(w),nextpow2(h));
            cdebug(mTexture);
            if(s)
              {
                mTexture->setSurface(s);
                s->glTexture=mTexture;
                version=s->version;
              }
          }
      }
    else
      {
        STACKTRACE;
        // check if resident
        //      GLuint id=mTexture->id();

#if ((CPU_FAMILY != PPC) && (CPU_FAMILY != POWERPC))
        GLboolean b;
        GLboolean r=glAreTexturesResident(1,&id,&b); // gives strange results on r300/ppc
        if(r)
          if(!b)
            {
              std::cerr<<"Texture "<<id<<" is not resident - maybe you should lower the texture-level"<<std::endl;
              std::cerr<<"Check your ~/.Antargis/config.xml !"<<std::endl;
              getSurfaceManager()->cleanup(true,true);
            }
#endif
      }
    mTextureUsed=true;
    return mTexture;
  }

bool AGTexture::textureUsed() const
{
  return mTextureUsed;
}

void AGTexture::beginPaint()
  {
    assert(!isRendering());
    assert(!m3d);
    if(opengl())
      {
        glDisable(GL_SCISSOR_TEST); // to make sure

        if(canFBO())
          {
            getScreen().begin();
            if(!mFBO)
              mFBO=new AGFBO(glTexture());

            mFBO->beginDraw();

            AGRenderContext nc;
            nc.begin();

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            if(mCleared)
              glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mCleared=false;
          }
        else
          {
            // init 2d drawing
            getScreen().begin();

            // do this so that texture generated
            AGRenderContext c;
            c.setColor(AGVector4(1,1,1,1));
            c.setTexture(glTexture());
            c.setCulling(false);
            c.begin();

            AGRenderContext nc;
            nc.begin();

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            mPainting=true;
            if(!mCleared)
              blit(*this,getRect().origin(),getRect().origin());
            mCleared=false;
          }
        glDisable(GL_SCISSOR_TEST); // to make sure
      }
    else
      {
        // create screen-like surface
        if(!s)
          {
            AGSurface s(w,h);
            *this=AGTexture(s);
          }
        sdlTexture();
      }

    mPainting=true;
  }
void AGTexture::endPaint()
  {
    if(opengl())
      {

        if(canFBO())
          {
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            assert(mFBO);
            mFBO->endDraw();
          }
        else
          {
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            bindTexture();

            // copy buffer to texture

            glReadBuffer(GL_BACK);

            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, mTexture->width(),mTexture->height(),0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          }

      }
    mPainting=false;
  }

void AGTexture::putPixel(int x,int y,const AGColor &c)
  {
    assert(mPainting);
    if(opengl())
      {
        AGRenderContext rc;
        rc.begin();
        float f=0xFF;
        rc.setColor(AGVector4(c.r/f,c.g/f,c.b/f,c.a/f));
        assert(mTexture);
        rc.begin();

        glBegin(GL_POINTS);
        glVertex2f(x,y);//getScreen().getHeight()-y);
        glEnd();

      }
    else
      {
        sdlTexture();
        sge_PutPixel(mSDLTexture->surface,x,y,c.mapRGB(mSDLTexture->surface->format));


        sge_PutPixel(s->surface,x,y,c.mapRGB(s->surface->format));
      }
  }


void AGTexture::fillRect(const AGRect2 &pRect,const AGColor &c)
  {
    if(opengl())
      {
        assert(mTexture);
        cdebug(pRect);
        AGGLPainter::fillRect(pRect,c);
      }
    else
      {
        for(int x=(int)pRect.x0();x<=pRect.x1();x++)
          for(int y=(int)pRect.y0();y<=pRect.y1();y++)
            putPixel(x,y,c);

        //      throw std::runtime_error("implement fillRect for sdl-texture");
      }

  }

//FIXME: del this
extern size_t gUsedTexMemory;

void AGTexture::blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc)
  {
    if(opengl())
      {
        assert(mTexture);
        AGGLPainter::blit(pSource,pDest,pSrc,AGColor(0xFF,0xFF,0xFF));
      }
    else
      {
        SDL_Rect sr,dr;
        sr=pSrc.sdl();
        dr=pDest.sdl();

        const_cast<AGTexture&>(pSource).sdlTexture();

        if(SDL_BlitSurface(pSource.s->surface,&sr,s->surface,&dr))
          cdebug("ERROR");

        Uint32 saved_flags;
        Uint8  saved_alpha;

        SDL_Surface *sourceSurface;

        if(pSource.mSDLTexture)
          sourceSurface=pSource.mSDLTexture->surface;
        else
          sourceSurface=pSource.s->surface;

        // save alpha-settings
        saved_flags = sourceSurface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
        saved_alpha = sourceSurface->format->alpha;

        // clear alpha-settings, so that rgba is copied completely
        SDL_SetAlpha(sourceSurface,0,0);

        if(SDL_BlitSurface(sourceSurface,&sr,sdlTexture()->surface,&dr))
          cdebug("ERROR Blitting");

        // restore settings
        SDL_SetAlpha(sourceSurface,saved_flags,saved_alpha);
      }
  }
void AGTexture::blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor)
  {
    if(opengl())
      {
        assert(mTexture);
        AGGLPainter::blit(pSource,pDest,pSrc,pColor);
      }
    else
      throw std::runtime_error("implement blitting for sdl-texture");
  }
void AGTexture::drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c)
  {
    if(opengl())
      {
        assert(mTexture);

        AGGLPainter::drawLine(p0,p1,c,2);
      }
    else
      AGSDLPainter::drawLine(mSDLTexture->surface,p0,p1,c);
    //    throw std::runtime_error("implement drawLine for sdl-texture");
  }

void AGTexture::drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    if(opengl())
      {
        assert(mTexture);
        AGGLPainter::drawGradientAlpha(rect,ul,ur,dl,dr);
      }
    else
      AGSDLPainter::drawGradientAlpha(mSDLTexture->surface,rect,ul,ur,dl,dr);
    //    throw std::runtime_error("implement drawLine for sdl-texture");
  }

void AGTexture::drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
  {
    if(opengl())
      {
        assert(mTexture);
        AGGLPainter::drawGradientAlpha(rect,ul,ur,dl,dr);
      }
    else
      AGSDLPainter::drawGradientAlpha(mSDLTexture->surface,rect,ul,ur,dl,dr);
    //    throw std::runtime_error("implement drawLine for sdl-texture");
  }


void AGTexture::setWrapping(bool pWrap)
  {
    bindTexture();
    //  if(m3d && pWrap)
    //    setClamp(GL_REPEAT,GL_REPEAT,GL_CLAMP_TO_EDGE );
  }
void AGTexture::setFilter(GLuint mag,GLuint min)
  {
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min);
  }

void AGTexture::bindTexture()
  {
    AGRenderContext c;
    c.setTexture(glTexture());
    c.begin();
  }

float AGTexture::getTW() const
{
  if(mTexture)
    return float(w)/float(mTexture->width());
  return 1;
}
float AGTexture::getTH() const
{
  if(mTexture)
    return float(h)/float(mTexture->height());
  return 1;
}

float AGTexture::getSurfaceWidth() const
{
  return float(w);
}
float AGTexture::getSurfaceHeight() const
{
  return float(h);
}

float AGTexture::getTextureWidth() const
{
  return float(const_cast<AGTexture*>(this)->glTexture()->width());
}
float AGTexture::getTextureHeight() const
{
  return float(const_cast<AGTexture*>(this)->glTexture()->height());
}

bool AGTexture::is3d() const
{
  return m3d;
}

void AGTexture::setClamp(GLuint s,GLuint t,GLuint r)
  {
    if(!opengl())
      return;
    if(is3d())
      {
        bindTexture();
        glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, s);
        glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, t);
        glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, r);
      }
    else
      {
        bindTexture();
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
      }
  }

void AGTexture::useTexture()
  {
    mTextureUsed=true;
  }

AGTexture AGTexture::copy() const
{
  throw std::runtime_error("not implemented!");
}


/// clears texture-content when you paint on a texture in gl-mode
/// you must call this before creating the AGPainter object
void AGTexture::clearContent()
  {
    mCleared=true;
  }


AGSurface AGTexture::getSurface() const
{
  if(mTexture)
    return mTexture->getSurface();
  return AGSurface();
}
