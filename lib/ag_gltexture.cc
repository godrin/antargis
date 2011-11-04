/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_gltexture.cc
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

#include <stdexcept>
#include "ag_gltexture.h"
#include "ag_surfacemanager.h"
#include "ag_rendercontext.h"
#include "rk_debug.h"
#include "ag_vdebug.h"

#include "ag_surface_internal.h"


//FIXME: this should be static
size_t gUsedTexMemory=0;

bool isPowerOf2(size_t value)
  {
    while(value!=0)
      {
        if(value&1)
          {
            if((value^1))
              return false;
            else
              return true;
          }
        value>>=1;

      }
    return true;
  }
size_t nextPowerOf2(size_t value)
  {
    TRACE;
    size_t c=1;
    while(c<value)
      {
        cdebug(c<<";"<<value);
        c<<=1;
      }
    return c;
  }


char *AGGLTexture::dmaBuffer=0;

AGGLTexture::AGGLTexture(size_t W,size_t H,GLint format):w(W),h(H),d(1),m3d(false),mRectTex(false),mTarget(GL_TEXTURE_2D)
  {
    //cdebug("HUPE");
    initDMABuffer();
    assertGL;
    getSurfaceManager()->registerMe(this);
    assertGL;
    glGenTextures( 1, &mID);
    assertGL;
    bool inited=false;
    mTarget=GL_TEXTURE_2D;

    //cdebug(w<<":"<<h);
    if(!(isPowerOf2(w) && isPowerOf2(h)))
      {
        //cdebug("not power of 2");
        if(GLEE_ARB_texture_rectangle && false)
          {
            mTarget=GL_TEXTURE_RECTANGLE_ARB;
          }
        else
          {
            h=w=nextPowerOf2(std::max(w,h));
          }
      }
    //cdebug("1");
    glEnable(mTarget);
    //cdebug("2");
    glBindTexture( mTarget,mID);
    assertGL;

    size_t bufSize=w*h*4;
    char *buffer=0;

    if(false)
      {
        buffer=new char[bufSize];

        assert(buffer);
      }
    if(buffer)
      memset(buffer,0,bufSize);
    assertGL;
    glTexImage2D(mTarget, 0, format, w, h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, buffer);
    assertGL;

    if(buffer)
        checkedDeleteArray(buffer);

    assertGL;
    gUsedTexMemory+=w*h*4;

    dbout(4,"used memory:"<<gUsedTexMemory);

    glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assertGL;
    glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assertGL;
  }
AGGLTexture::AGGLTexture(size_t W,size_t H,size_t D,GLint format):w(W),h(H),d(D),m3d(true),mRectTex(false),mTarget(GL_TEXTURE_3D)
  {
    assertGL;
    getSurfaceManager()->registerMe(this);
    assertGL;

    glGenTextures( 1, &mID);
    assertGL;
    glBindTexture( mTarget,mID);
    assertGL;

    glTexImage3D(mTarget, 0, format, w, h, d, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, 0);
    assertGL;

    gUsedTexMemory+=w*h*d*4;
    dbout(4,"used memory:"<<gUsedTexMemory);

    glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assertGL;
    glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assertGL;
  }

AGGLTexture::~AGGLTexture()
  {
    assertGL;
    glDeleteTextures(1,&mID);

    if(m3d)
      gUsedTexMemory-=w*h*d*4;
    else
      gUsedTexMemory-=w*h*4;

    assertGL;
    getSurfaceManager()->deregisterMe(this);
  }

void AGGLTexture::setSurface(AGInternalSurface *pSurface,const AGVector2 &offset)
  {
    assertGL;
    GLint format;

    SDL_Surface *surface=pSurface->surface;

    switch(surface->format->BytesPerPixel)
      {
    case 3:
      format = GL_RGB;break;
    case 4:
      format = GL_RGBA;break;
    default:
      throw std::runtime_error("Surface-format not supported for texturing!");
      }

    AGRenderContext c;
    c.setTexture(this);
    c.begin();
    assertGL;


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    assertGL;
    glPixelStorei(GL_UNPACK_ROW_LENGTH,
        surface->pitch / surface->format->BytesPerPixel);


    assertGL;
    if(m3d)
      {
        assert(glIsEnabled(mTarget));
        int mw=std::min(surface->w,surface->h);
        int mh=std::max(surface->w,surface->h)/mw;

        // FIXME: use glTexImage instead of glTexSubImage, because it makes problems
        // on MacOSX 10.5.1 (at least on my macbook pro - godrin)

        glTexImage3D(mTarget, 0, format, w, h, d, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, surface->pixels);
        glTexSubImage3D(mTarget,0,int(offset[0]),int(offset[1]),0,
            mw,mw,mh,format,GL_UNSIGNED_BYTE,surface->pixels);
      }
    else
      {
        assertGL;

        glTexSubImage2D(mTarget, 0, int(offset[0]), int(offset[1]),
            surface->w, surface->h, format, GL_UNSIGNED_BYTE,
            surface->pixels);
      }
    assertGL;
    AGRenderContext().begin();
  }

GLuint AGGLTexture::id()
  {

    return mID;
  }
bool AGGLTexture::is3d() const
{
  return m3d;
}

size_t AGGLTexture::width() const
{
  return w;
}
size_t AGGLTexture::height() const
{
  return h;
}
size_t AGGLTexture::depth() const
{
  return d;
}

AGRect2 AGGLTexture::getRect() const
{
  return AGRect2(0,0,w,h);
}


AGSurface AGGLTexture::getSurface() const
{
  AGSurface s(w,h*d);
  size_t bufSize=width()*height()*4;
  unsigned char *pixels=new unsigned char[bufSize];

  glGetTexImage(mTarget,0,GL_RGBA,
      GL_UNSIGNED_INT_8_8_8_8_REV,pixels);

  {
    // copy
    TRACE;
    int h=height()-1;
    for(int x=0;x<(int)width();x++)
      for(int y=0;y<(int)height();y++)
        {
          unsigned char*p=pixels+((x+(h-y)*width())*4);
          AGColor c(p[0],p[1],p[2],p[3]);
          s.putPixel(x,y,c);
        }
  }

  return s;
}

void AGGLTexture::initDMABuffer()
  {
    return;
    /*
    if(!dmaBuffer)
      {
        size_t texbufsize=1024*1024*64;
        dmaBuffer=new char[texbufsize];
        glTextureRangeAPPLE(GL_TEXTURE_2D,texbufsize,dmaBuffer);
      }
      */
  }
