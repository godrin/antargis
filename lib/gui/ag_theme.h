/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_theme.h
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

#ifndef AG_THEME_H
#define AG_THEME_H

#include "ag_font.h"
#include "ag_surface.h"
#include "rk_base.h"

#include "ag_background.h"
#include "ag_border.h"
#include "ag_gradient.h"

#include <map>

class AGEXPORT AGLocalTheme {
public:
  AGLocalTheme(const AGString &pTheme);
  virtual ~AGLocalTheme() throw();

  AGFont getFont(const AGString &pName) const;
  AGColor getColor(const AGString &pName) const;
  int getInt(const AGString &pName) const;
  AGSurface getSurface(const AGString &pName) const;
  std::string getSurfaceName(const AGString &pName) const;

  bool hasFont(const AGString &pName) const;
  bool hasColor(const AGString &pName) const;
  bool hasInt(const AGString &pName) const;
  bool hasSurface(const AGString &pName) const;
  bool hasSurfaceName(const AGString &pName) const;

private:
  AGString getName(const AGString &pName) const;

  AGString mTheme;
};

class AGEXPORT AGTheme {
public:
  AGTheme();
  virtual ~AGTheme() throw();

  AGFont getFont(const AGString &pName) const;
  bool hasFont(const AGString &pName) const;
  void setFont(const AGString &pName, AGFont pFont);

  void setColor(const AGString &pName, AGColor pColor);
  AGColor getColor(const AGString &pName) const;
  bool hasColor(const AGString &pName) const;

  int getInt(const AGString &pName) const;
  bool hasInt(const AGString &pName) const;
  void setInt(const AGString &pName, int i);

  AGSurface getSurface(const AGString &pName) const;
  bool hasSurface(const AGString &pName) const;
  void setSurface(const AGString &pName, const AGSurface &pSurface);

  std::string getSurfaceName(const AGString &pName) const;
  bool hasSurfaceName(const AGString &pName) const;
  void setSurfaceName(const AGString &pName, const std::string &pSurface);

  AGLocalTheme getTheme(const AGString &pTheme) const;

  // advanced functions

  AGGradient getGradient(const AGString &pName) const;
  AGBackground getBackground(const AGString &pName) const;
  AGBorder getBorder(const AGString &pName) const;

private:
  AGString trunc(AGString s) const;

  std::map<AGString, AGFont> mFonts;
  std::map<AGString, AGColor> mColors;
  std::map<AGString, AGSurface> mSurfaces;
  std::map<AGString, std::string> mSurfaceNames;
  std::map<AGString, int> mInts;

  std::map<AGString, AGGradient> mGradients;
  std::map<AGString, AGBackground> mBackgrounds;
  std::map<AGString, AGBorder> mBorders;
};

AGEXPORT AGTheme *getTheme();
AGEXPORT void setTheme(const AGTheme &aTheme);
AGEXPORT void loadTheme(const AGString &pXML);
AGEXPORT bool loadThemeFile(const AGString &pFilename);

AGEXPORT AGString addPoint(const AGString &pTheme);

#endif
