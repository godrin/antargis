/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_texture.h
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

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef AG_TEXTURE_H
#define AG_TEXTURE_H

#include "ag_surface.h"
#include "ag_gltexture.h"
#include "ag_painttarget.h"

class AGFBO;

class AGEXPORT AGTexture:public AGPaintTarget
{
 public:
  AGTexture();
  AGTexture(const AGSurface &pSurface,bool p3d=false);
  AGTexture(const AGTexture &t);         // shallow copy, uses same tex-id or sdl-surface
  AGTexture(int W,int H);
  ~AGTexture() throw();

#ifndef SWIG
  AGTexture &operator=(const AGTexture &t);
#endif

  AGTexture copy() const; // returns a real copy

  int width() const;
  int height() const;

  AGRect2 getRect() const;

  AGColor getPixel(int x,int y) const;

  float getSurfaceWidth() const;
  float getSurfaceHeight() const;

  float getTextureWidth() const;
  float getTextureHeight() const;

  float getTW() const;
  float getTH() const;

  bool hasTexture() const;
  bool textureUsed() const;

  void clearTexture();
  void clearTextureUsed();

  void useTexture();
 
  virtual void beginPaint();
  virtual void endPaint();

  virtual void putPixel(int x,int y,const AGColor &c);
  virtual void fillRect(const AGRect2 &pRect,const AGColor &c);
  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc);
  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor);
  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c);
  virtual void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  virtual void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);


  void setWrapping(bool pWrap);
  void setFilter(GLuint mag,GLuint min);
  void setClamp(GLuint s,GLuint t,GLuint r);

  AGGLTexture *glTexture();
  AGInternalSurface *sdlTexture();

  bool is3d() const;

  void clearContent();
  
  AGSurface getSurface() const;

 private:

  void bindTexture();

  AGGLTexture *mTexture;
  bool mTextureUsed;
  bool mPainting;
  bool mCleared;

  bool m3d;
  int w,h;

  AGInternalSurface *s;
  AGInternalSurface *mSDLTexture;

  AGFBO *mFBO; // used for render-to-texture

  size_t version;
  

  friend class AGTexturePainter;
  friend class AGSDLScreen;
  friend class AGGLScreen;
};


#endif
