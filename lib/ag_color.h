#/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_color.h
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

#ifndef AG_COLOR_H
#define AG_COLOR_H

#include <SDL.h>
#include <iostream>
#include <rk_base.h>
#include <rk_string.h>

class AGSurface;
class AGVector4;

class AGEXPORT AGColor:public SDL_Color
{
 public:
  AGColor(int pr,int pg,int pb,int pa=255);

  AGColor(const AGColor &c);
  AGColor(const AGVector4 &v);
  AGColor(Uint32 c,const AGSurface &pSurface);
  AGColor(const AGString &s);

  AGColor();

#ifndef SWIG
  AGColor &operator=(const AGColor&c);
  AGColor &operator*=(float f);
#endif

  AGColor operator*(float f) const;
  AGColor operator+(const AGColor &c) const;

  Uint32 mapRGB(SDL_PixelFormat *f) const;

  AGString toString() const;

  AGVector4 toVec() const;

  AGColor grey() const;

  Uint8 brightness() const;

  void light(Uint8 l);

  //  SDL_Color sdlColor() const;

  bool operator==(const AGColor &c) const;

  //  Uint8 r,g,b;
  Uint8 a;
};

#ifndef SWIG
bool operator<(const AGColor &c1,const AGColor &c2);
bool operator==(const AGColor &c1,const AGColor &c2);
bool operator!=(const AGColor &c1,const AGColor &c2);

std::ostream &operator<<(std::ostream &o,const AGColor &c);
#endif

inline int toInt(Uint8 u)
{
  return u;
}

#endif
