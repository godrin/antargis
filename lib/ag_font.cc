/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_font.cc
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

#include "ag_font.h"
#include "ag_fontengine.h"
#include <sstream>

AGFont::AGFont(AGFontEngine *pEngine,const std::string &pFile,int size):
engine(pEngine),
  mName(pFile),mSize(size),mColor(0xFF,0xFF,0xFF)
  {
    mAlpha=0xFF;
    mStyle=NORMAL;
    borderSize=0;
    embossed=false;
    inset=false;
  }

AGFont::AGFont(AGFontEngine *pEngine)
  {
    engine=pEngine;
    mName="FreeSans.ttf";
    mSize=14;
    mColor=AGColor(0xFF,0xFF,0xFF);
    mAlpha=0xFF;
    mStyle=NORMAL;
    borderSize=0;
    embossed=false;
    inset=false;
  }

AGFont &AGFont::operator=(const AGFont &f)
  {
    engine=f.engine;
    mName=f.mName;
    mSize=f.mSize;
    mColor=f.mColor;
    mBorderColor=f.mBorderColor;
    mAlpha=f.mAlpha;
    mStyle=f.mStyle;
    borderSize=f.borderSize;
    embossed=f.embossed;
    inset=f.inset;
    return *this;
  }

void AGFont::setColor(const AGColor &pColor)
  {
    mColor=pColor;
  }
AGColor AGFont::getColor() const
{
  return mColor;
}

void AGFont::setAlpha(Uint8 pAlpha)
  {
    mAlpha=pAlpha;
  }
Uint8 AGFont::getAlpha() const
{
  return mAlpha;
}

void AGFont::setSize(Uint8 pSize)
  {
    mSize=pSize;
  }
Uint8 AGFont::getSize() const
{
  return mSize;
}

void AGFont::setStyle(const Style &s)
  {
    mStyle=s;
  }

AGFont::Style AGFont::getStyle() const
{
  return mStyle;
}

void AGFont::setName(const std::string &s)
  {
    mName=s;
  }
std::string AGFont::getName() const
{
  return mName;
}

int AGFont::getWidth(const AGStringUtf8 &mText) const
{
  return engine->getWidth(*this,mText);
}
int AGFont::getHeight(const AGStringUtf8 &mText) const
{
  return engine->getHeight(*this,mText);
}

void AGFont::setEmbossed(bool e)
  {
    embossed=e;
  }
bool AGFont::getEmbossed() const
{
  return embossed;
}

void AGFont::setBorderColor(const AGColor &pColor)
  {
    mBorderColor=pColor;
  }
AGColor AGFont::getBorderColor() const
{
  return mBorderColor;
}

void AGFont::setBorder(int i)
  {
    borderSize=i;
  }
int AGFont::getBorder() const
{
  return borderSize;
}

void AGFont::setInset(bool e)
  {
    inset=e;
  }
bool AGFont::getInset() const
{
  return inset;
}

std::string AGFont::toString() const
{
  std::ostringstream os;

  os<<"("<<mName<<" size:"<<(int)mSize<<")";

  return os.str();
}



#define COMPARE(x,y) if(x!=y) return x<y

bool operator<(const AGFont&f1,const AGFont &f2)
  {
    COMPARE(f1.mStyle,f2.mStyle);
    COMPARE(f1.mName,f2.mName);
    COMPARE(f1.mSize,f2.mSize);
    COMPARE(f1.mAlpha,f2.mAlpha);
    COMPARE(f1.mColor,f2.mColor);
    COMPARE(f1.embossed,f2.embossed);
    COMPARE(f1.inset,f2.inset);
    COMPARE(f1.mBorderColor,f2.mBorderColor);
    COMPARE(f1.borderSize,f2.borderSize);
    return false;
  }
