/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_glpainter.h
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

#ifndef AG_GLPAINTER
#define AG_GLPAINTER

#include <ag_geometry.h>

class AGTexture;
class AGColor;

#include <vector>

/** This module provides painting on OpenGL, which is used by AGGLScreen and AGTexture (that's the reason it's here).
*/
namespace AGGLPainter
{
  void blit(const AGTexture &pSource,const AGRect2 &pRect,const AGRect2 &pSrc,const AGColor &pColor);
  void blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor);

  void tile(const AGTexture &pSource,const AGRect2 &pTarget,const AGColor &pColor);
  
  void fillRect(const AGRect2 &pRect,const AGColor &c);
  void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pRects);
  void fillPoly(const std::vector<AGVector2> &pVecs,const AGColor &pColor);

  void drawPoly(const std::vector<AGVector2> &pVecs,const AGColor &pColor,float pWidth);

  void drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2);

  void drawGradientAlpha(const AGRect2& pRect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);

  void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c,float pWidth);

  void blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest);
  void blit3dTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle3 &pDest);

  void putPixel(int x,int y,const AGColor &pc);
}


#endif
