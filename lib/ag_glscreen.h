/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_glsurface.h
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

#ifndef __GUI_GLSURFACE_H
#define __GUI_GLSURFACE_H

#include <rk_base.h>

#include <ag_geometry.h>
#include "ag_screen.h"
#include "ag_gl.h"

#include <string>
#include <list>
#include <set>
#include <SDL.h>

class AGColor;

typedef GLuint TextureID;

class AGEXPORT AGGLObject
{
 public:
  AGGLObject();
  virtual ~AGGLObject();

  virtual void onScreenDown();
  virtual void onScreenUp();
};

class AGEXPORT AGGLScreen:public AGScreen
{
 public:
  AGGLScreen(int W,int H,int VW=-1,int VH=-1);

  virtual ~AGGLScreen() throw();

  virtual void blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest);

  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc);
  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor);


  virtual void blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor);

  virtual void tile(const AGTexture &pSource,const AGRect2 &pTarget,const AGColor &pColor);


  virtual void fillRect(const AGRect2 &pRect,const AGColor &c);
  virtual void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pRects);
  virtual void fillPoly(const std::vector<AGVector2> &pPoly,const AGColor &pColor);
  virtual void drawPoly(const std::vector<AGVector2> &pPoly,const AGColor &pColor);
  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c);

  void blit3dTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle3 &pDest);

  virtual AGRect2 getRect() const;

  virtual void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);

  virtual void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  virtual void drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2);
  virtual void putPixel(int x,int y,const AGColor &c);

  virtual void begin(); // call before start drawing

  AGSurface screenshotSurface(bool frontBuffer=true);
  AGTexture screenshot(bool frontBuffer=true);

  void flip();
  virtual void update(const std::list<AGRect2> &rs); // call this instead of flip, if you want
  bool inScreen(const AGRect2 &r) const;

  virtual size_t getWidth() const;
  virtual size_t getHeight() const;

  void clip(const AGRect2 &r);
  void unclip();

  void beginPaint();
  void endPaint();

  void setLineWidth(float w);

  void screenDown();
  void screenUp();

  static void addGLObject(AGGLObject *pObject);
  static void removeGLObject(AGGLObject *pObject);

 private:
  AGRect2 getRect(SDL_Surface *s);
  void checkUnusedTextures();

  int w,h;
  int rw,rh;

  float mLineWidth;

  friend class AGTexture;

  static std::set<AGGLObject*> msObjects;
};

#endif
