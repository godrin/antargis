/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_caption.cc
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

#include "ag_caption.h"
#include "rk_debug.h"
#include "ag_screen.h"

AGCaption::AGCaption(AGWidget *pParent,const AGRect2 &pRect,const AGStringUtf8 &pText,const AGFont &pFont,const AGBackground &pBG):
  AGText(pParent,pRect,pText,pFont),
  mBG(pBG)
  {
  }

void AGCaption::draw(AGPainter &p)
  {
    // draw bg
    mBG.draw(getRect().origin(),p);
    AGText::draw(p);
  }

void AGCaption::setBackground(AGBackground pBG)
  {
    mBG=pBG;
  }
