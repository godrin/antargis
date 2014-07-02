/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_gsurface.h
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

#ifndef __GUI_SURFACE_H
#define __GUI_SURFACE_H

#include "rk_exception.h"

#include <string>
#include <set>
#include <list>
#include <SDL.h>
#include "ag_gl.h"
#include "ag_geometry.h"
#include "ag_painttarget.h"
#include "ag_gltexture.h"
#include "rk_exception.h"

class AGColor;
class AGSurface;
class AGGLTexture;
struct AGInternalSurface;

AGEXPORT void beginRender();
AGEXPORT void endRender();
AGEXPORT bool isRendering();

class AGSurfaceEmptyException:public AGException
{
public:
  AGSurfaceEmptyException():AGException("Surface is used but empty!")
  {
  }

};

/// Generic classes - for OpenGL and "normal" SDL
class AGEXPORT AGSurface:public AGPaintTarget
{
 public:
  AGSurface();
  AGSurface(int w,int h);
  AGSurface(const AGSurface &p); // shallow copy only

  virtual ~AGSurface() throw();

  AGSurface copy() const;

  int width() const throw(AGSurfaceEmptyException);
  int height() const throw(AGSurfaceEmptyException);

  AGRect2 getRect() const throw(AGSurfaceEmptyException);

  AGSurface getSubSurface(const AGRect2 &r) const throw(AGSurfaceEmptyException);

  bool valid() const;

  AGSurface scale(int w,int h) const;
  AGSurface grayed() const;
#ifndef SWIG
  AGSurface &operator=(const AGSurface &p);
  #endif
  bool operator==(const AGSurface &p) const;

  virtual void blit(const AGSurface &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor) throw(AGSurfaceEmptyException);

  void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr) throw(AGSurfaceEmptyException);
  void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr) throw(AGSurfaceEmptyException);
  void drawBorder(const AGRect2& rect,int width, const AGColor& c1, const AGColor& c2) throw(AGSurfaceEmptyException);

  virtual void putPixel(int x,int y,const AGColor &c);
  virtual AGColor getPixel(int x,int y) const;

  virtual void fillRect(const AGRect2 &pr,const AGColor &c);
  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c);


  void save(const std::string &pName) const;

  static AGSurface load(const std::string &pName) throw(FileNotFound);

  AGInternalSurface *surface() const;

  AGVector2 shrink2Fit(int alphaThresh=20);

  AGSurface(AGInternalSurface *i);

  float similarity(const AGSurface &pSurface) const;
  bool similarTo(const AGSurface &pSurface,float threshold=0.1) const;



 private:

  Uint32 color(const AGColor &c) const throw(AGSurfaceEmptyException);;

  AGInternalSurface *s;

  friend class AGFontEngine;
  friend class AGSDLScreen;
  friend class AntTextureData;
  size_t mVersion;
};

AGEXPORT void AGFreeSurface(SDL_Surface *s);

AGEXPORT std::ostream &operator<<(std::ostream &o,SDL_PixelFormat *f);
AGEXPORT std::ostream &operator<<(std::ostream &o,SDL_Surface *f);

#endif
