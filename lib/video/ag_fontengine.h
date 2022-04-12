/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_fontengine.h
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

#ifndef AG_FONTENGINE_H
#define AG_FONTENGINE_H

#include <map>
#include <string>

#include <ag_font.h>
#include <ag_geometry.h>
#include <ag_surface.h>

class AGEXPORT AGFontEngine {
public:
  AGFontEngine();
  ~AGFontEngine() throw();

  AGTexture *renderText(int BaseLineX, int BaseLineY, const AGStringUtf8 &pText,
                        const AGFont &ParamIn);

  int getWidth(const AGFont &pFont, const AGStringUtf8 &pText);
  int getHeight(const AGFont &pFont, const AGStringUtf8 &pText);

private:
  std::map<std::pair<AGFont, AGStringUtf8>, AGTexture *> fontCache;
};

AGFontEngine *getFontEngine();

#endif
