/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_border.h
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

#ifndef __AG_BORDER_H__
#define __AG_BORDER_H__

#include <string>
#include "ag_texture.h"
#include "ag_geometry.h"

class AGPainter;


class AGEXPORT AGBorder
{
 public:
  AGBorder(const AGString &pTheme="");
  AGBorder(const AGSurface &pSurface,const AGString &pSurfaceName);

  void draw(const AGRect2 &r,AGPainter &p);

  size_t getWidth() const;

  void useTextures();

 private:
  const AGTexture *mTexture;

  std::vector<const AGTexture*> mTextures;

  AGString mTheme;
  bool mEnable;
  size_t mWidth;
};

#endif
