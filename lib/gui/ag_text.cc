/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_text.cc
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


#include "ag_text.h"
#include "rk_debug.h"
#include "ag_surface.h"
#include "ag_fontengine.h"
#include "ag_geometry.h"

#include <iostream>


using namespace std;

AGText::AGText(AGWidget *pParent,const AGRect2 &r,const AGStringUtf8 &pText,const AGFont &pFont):
  AGWidget(pParent,r),
  mText(pText),mFont(pFont)
  {
    mFixedSize=true;
  }

AGText::AGText(AGWidget *pParent,const AGVector2 &p,const AGStringUtf8 &pText,const AGFont &pFont):
  AGWidget(pParent,AGRect2(p[0],p[1],pFont.getWidth(pText),pFont.getHeight(pText))),
  mText(pText),mFont(pFont)
  {
    mFixedSize=false;
  }

AGText::~AGText() throw()
  {}


void AGText::draw(AGPainter &p)
  {
    float x=(width()-mFont.getWidth(mText))/2;
    float y=(height()-mFont.getHeight(mText))/2;
    p.renderText(mText,AGVector2((int)x,(int)y),mFont);
  }

void AGText::setDeriveRect()
  {
    setRect(AGRect2(getRect().x(),getRect().y(),mFont.getWidth(mText),mFont.getHeight(mText)));
  }

void AGText::setText(const AGStringUtf8 &pText)
  {
    mText=pText;
    if(!mFixedSize)
      {
        setWidth(mFont.getWidth(pText));
        setHeight(mFont.getHeight(pText));
      }
  }
AGStringUtf8 AGText::getText() const
{
  return mText;
}

void AGText::setFont(const AGFont &f)
  {
    mFont=f;
  }
