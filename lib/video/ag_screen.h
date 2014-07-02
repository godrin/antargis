/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_screen.h
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

#ifndef AG_SCREEN_H
#define AG_SCREEN_H

#include <rk_base.h>

#include <ag_config.h>
#include <ag_painttarget.h>

// this is virtually a pure virtual class ;-)
// because swig directors have problems with this otherwise

class AGEXPORT AGScreen:public AGPaintTarget
{
 public:
   AGScreen();
  virtual ~AGScreen() throw();
  virtual void begin(); // call before start drawing
  virtual void flip();
  virtual void update(const std::list<AGRect2> &rs); // call this instead of flip, if you want

  virtual size_t getWidth() const;
  virtual size_t getHeight() const;

  virtual AGSurface screenshotSurface(bool frontBuffer=true);
  virtual AGTexture screenshot(bool frontBuffer=true);
  virtual AGFontEngine *getFontEngine();
private:
  AGFontEngine *mFontEngine;
};

AGEXPORT AGScreen &getScreen();
AGEXPORT void setScreen(AGScreen *s);

#endif
