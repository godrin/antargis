/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_sdlsurface.h
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

#ifndef __GUI_SDLSURFACE_H
#define __GUI_SDLSURFACE_H

#include "ag_screen.h"

class AGSDLScreen:public AGScreen
{
 public:
  AGSDLScreen(SDL_Surface *s);

  virtual void blit(const AGTexture &pSource,const AGRect2 &pDest,const AGRect2 &pSrc);

  virtual void flip();
  virtual void update(const std::list<AGRect2> &rs);

  virtual AGRect2 getRect() const;

  virtual void fillRect(const AGRect2 &pRect,const AGColor &c);
  virtual void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pRects);
  virtual void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c);

  virtual void drawGradientAlpha(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  virtual void drawGradient(const AGRect2& rect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);

  virtual void drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2);

  virtual void putPixel(int x,int y,const AGColor &c);

  //  virtual SDL_Surface *newSurface(int x,int y);

  //  virtual AGSurface loadSurface(const std::string &pFilename);

  //  virtual AGTexture displayFormat(SDL_Surface *s);

  //  virtual AGTexture makeTexture(const AGSurface &s);

  virtual size_t getWidth() const;
  virtual size_t getHeight() const;

  virtual void clip(const AGRect2 &r);
  virtual void unclip();

  AGSurface screenshotSurface(bool frontBuffer=true);
  AGTexture screenshot(bool frontBuffer=true);

 private:
  SDL_Surface *s;
 public:
};

#endif
