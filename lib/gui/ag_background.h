/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_background.h
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

#ifndef AG_BACKGROUND_H
#define AG_BACKGROUND_H

#include <string>
#include "ag_geometry.h"
#include "ag_texture.h"
#include "ag_color.h"

class AGPainter;
class AGLocalTheme;

/** AGBackground is a helper class for widget-drawing
    It is used to draw gradients and tiled backgrounds of widgets.
    It is themable.
 */
class AGEXPORT AGBackground
  {
  public:
    AGBackground(const AGString &pThemeName="");
    AGBackground(const AGLocalTheme &pTheme,const AGString &pThemeName="");
    AGBackground(const AGColor &pColor);

    void draw(const AGRect2 &r,AGPainter &p);

    void useTextures();
  private:
    
    void loadFromTheme(const AGLocalTheme &pTheme,const AGString &pThemeName="");
    
    const AGTexture *mTexture;
    AGColor mColors[4];

    bool mColor;
    int mBorder;
  };

#endif
