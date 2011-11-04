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

#ifndef __AG_CLIP_PAINTTARGET_H
#define __AG_CLIP_PAINTTARGET_H

#include <ag_painttarget.h>
#include <ag_clip.h>

/// virtual paint target - use this for advanced clipping !
class AGEXPORT AGClipPaintTarget:public AGPaintTarget
{
 public:
  AGClipPaintTarget(AGPaintTarget *pTarget);
  ~AGClipPaintTarget() throw();
  
  
  virtual void blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest);

  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc,const AGColor &pColor);
  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc);
  virtual void blit(const AGSurface &pSource,const AGRect2 &pDest,const AGRect2 &pSrc);

  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c);
  virtual void fillRect(const AGRect2 &pr,const AGColor &c);

  virtual void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pr);
   
  virtual void blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor);

  virtual void tile(const AGTexture &pSource,const AGRect2 &pTarget, const AGColor &pColor);

  virtual void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  virtual void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
   
  virtual AGRect2 getRect() const;

  virtual void putPixel(int x,int y,const AGColor &c);
  virtual AGColor getPixel(int x,int y) const;

  // manage painting sessions
  virtual void clip(const AGClipping &c);

  virtual void unclip();

 private:
  AGPaintTarget *mTarget;
  AGClipping mClipping;
};


#endif

