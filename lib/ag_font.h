/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_font.h
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

#ifndef __AG_FONT_H
#define __AG_FONT_H

#include <rk_base.h>
#include <rk_utf8.h>
#include <ag_color.h>

#include <ag_string_utf8.h>

class AGFontEngine;

class AGEXPORT AGFont
{
 public:

  enum Style {NORMAL,BOLD,UNDERLINE,ITALIC};

  AGFont(AGFontEngine *pEngine);
  AGFont(AGFontEngine *pEngine,const std::string &pFile,int size=14);//,int index=0);

  void setColor(const AGColor &pColor);
  AGColor getColor() const;

  void setBorderColor(const AGColor &pColor);
  AGColor getBorderColor() const;

  void setBorder(int i);
  int getBorder() const;

  void setAlpha(Uint8 pAlpha);
  Uint8 getAlpha() const;

  void setSize(Uint8 pSize);
  Uint8 getSize() const;

  void setStyle(const Style &s);
  Style getStyle() const;

  void setName(const std::string &s);
  std::string getName() const;

  void setEmbossed(bool e);
  bool getEmbossed() const;

  void setInset(bool e);
  bool getInset() const;

  int getWidth(const AGStringUtf8 &mText) const;
  int getHeight(const AGStringUtf8 &mText) const;

  AGFont &operator=(const AGFont &f);

  std::string toString() const;

 private:
   AGFontEngine *engine;
  Style mStyle;
  std::string mName;
  int mSize;
  Uint8 mAlpha;
  AGColor mColor;
  AGColor mBorderColor;
  int borderSize;
  bool embossed;
  bool inset;

#ifndef SWIG
  friend bool operator<(const AGFont&f1,const AGFont &f2);
#endif
};

AGEXPORT bool operator<(const AGFont&f1,const AGFont &f2);

#endif
