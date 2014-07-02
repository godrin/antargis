/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_painttarget.h
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

#ifndef __AG_PAINTTARGET_H
#define __AG_PAINTTARGET_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_geometry.h>
#include <ag_color.h>
#include <ag_font.h>

class AGSurface;
class AGTexture;

#define MSTUB { throw std::runtime_error("not implemented");}

class AGEXPORT AGPaintTarget
{
 public:
  virtual ~AGPaintTarget() throw()
    {
    }
  virtual void blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest) MSTUB

  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor) MSTUB
  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc) MSTUB
  virtual void blit(const AGSurface &pSource,const AGRect2 &pDest,const AGRect2 &pSrc) MSTUB

  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c) MSTUB
  virtual void fillRect(const AGRect2 &pr,const AGColor &c) MSTUB

  virtual void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pr) MSTUB

  virtual void fillPoly(const std::vector<AGVector2>&pPoly,const AGColor &pColor) MSTUB
  virtual void drawPoly(const std::vector<AGVector2>&pPoly,const AGColor &pColor) MSTUB

  virtual void blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor)
    MSTUB

  virtual void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr) MSTUB
  virtual void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr) MSTUB

  virtual void setLineWidth(float w) MSTUB

  virtual void tile(const AGTexture &pSource,const AGRect2 &pTarget, const AGColor &pColor) MSTUB

  virtual AGRect2 getRect() const
  {
    return AGRect2(0,0,0,0);
  }


  // modify single pixels - don't use this much!

  virtual void putPixel(int x,int y,const AGColor &c) MSTUB
  virtual AGColor getPixel(int x,int y) const MSTUB

  // manage painting sessions

  virtual void beginPaint()
  {}

  virtual void endPaint()
  {}

  virtual void clip(const AGRect2 &r)
  {}
  virtual void unclip()
  {}

};


#endif

