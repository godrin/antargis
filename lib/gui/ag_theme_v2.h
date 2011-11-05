/*
 * Copyright (c) 2008 by David Kamphausen. All rights reserved.
 *
 * ag_theme_v2.h
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

#ifndef AG_THEME_V2_H
#define AG_THEME_V2_H

#include "rk_base.h"

#include "ag_font.h"
#include "ag_surface.h"
#include "ag_gradient.h"
#include "ag_xml.h"
#include "ag_border.h"
#include "ag_background.h"

#include <map>

class AGThemeV2;

class AGEXPORT AGThemeItem
  {
    const AGThemeV2 *mTheme;
    Node *mNode;
public:
  AGBorder getBorder() const;
  AGBackground getBackground() const;
  AGFont getFont() const;

private:
  AGThemeItem(const AGThemeV2 *pTheme,Node *pNode);

  friend class AGThemeV2;
  };

class AGEXPORT AGThemeV2
  {
public:
  AGThemeV2(const std::string &pFilename,const std::string &pTheme="");
  virtual ~AGThemeV2();

  static AGThemeV2 *getTheme(const std::string &pFilename="",const std::string &pTheme="");

  AGFont getFont(const AGString &pName) const;
  AGColor getColor(const AGString &pName) const;
  int getInt(const AGString &pName) const;
  AGSurface getSurface(const AGString &pName) const;
  AGGradient getGradient(const AGString &pName) const;
  std::string getSurfaceName(const AGString &pName) const;

  bool hasSurface(const AGString &pName) const;
  bool hasColor(const AGString &pName) const;

  AGThemeItem getItem(const AGString &pClass,const AGString &pState) const;
  AGThemeItem getItem(const AGString &pClass,const AGString &pState,const AGString &pElement) const;

private:
#ifndef SWIG

  AGString mFilename;
  Document mDocument;
  AGString mTheme;

  void parseColors(const Node &pNode);
  void parseGradients(const Node &pNode);
  void parseFonts(const Node &pNode);
  void parseImages(const Node &pNode);

  bool checkColor(const AGString &p) const;

  template<class T>
  class Cont:public std::map<AGString,T>
  {};


  typedef Cont<AGFont> Fonts;
  typedef Cont<AGColor> Colors;
  typedef Cont<AGSurface> Surfaces;
  typedef Cont<AGGradient> Gradients;
  typedef Cont<int> Integers;
  typedef Cont<std::string> SurfaceNames;

  Fonts mFonts;
  Colors mColors;
  Surfaces mSurfaces;
  Gradients mGradients;
  Integers mIntegers;
  SurfaceNames mSurfaceNames;

  AGFont mDefaultFont;
  AGColor mDefaultColor;
  AGSurface mDefaultSurface;
  AGGradient mDefaultGradient;
  int mDefaultInteger;
  std::string mSurfaceName;

  static std::map<AGString,AGThemeV2*> gThemes;
#endif
  };

#endif
