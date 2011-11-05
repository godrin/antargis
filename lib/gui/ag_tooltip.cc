/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_tooltip.cc
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

#include "ag_tooltip.h"
#include "ag_theme.h"
#include "ag_screen.h"
#include "ag_edit.h"

#include "rk_debug.h"
#include "rk_tools.h"

AGTooltip::AGTooltip(const AGRect2 &pRect,const AGStringUtf8 &pText):
  AGWidget(0,pRect),
  mText(pText),
  mFont(getTheme()->getFont("tooltip.font")),
  mBgColor(getTheme()->getColor("tooltip.bgcolor")),
  mBorderColor(getTheme()->getColor("tooltip.bordercolor"))
  {

    float sw=getScreen().getWidth();
    float sh=getScreen().getHeight();

    mText=mText.replace("\\n","\n");

    float tw,th;

    if(mText.find("\n")!=std::string::npos)
      {
        // estimate each line size
        std::vector<AGStringUtf8> a=mText.split("\n");
        tw=th=0;
        for(std::vector<AGStringUtf8>::iterator i=a.begin();i!=a.end();++i)
          {
            tw=std::max(tw,(float)mFont.getWidth(*i));
            th+=mFont.getHeight(*i);
          }
      }
    else
      {
        // first estimate a rectangle which can hold the text
        float ftw=mFont.getWidth(mText);
        float fth=mFont.getHeight(mText);
        float maxw=150;
        if(ftw>maxw)
          {
            tw=maxw;
            th=(ftw+50)/maxw*(fth+4);
          }
        else
          {
            // 1 line only
            tw=ftw;
            th=fth;
          }
      }
    tw+=6;
    th+=5;

    // now estimate a nice position

    // check below

    cdebug("th:"<<th);
    cdebug("bottom:"<<bottom());
    cdebug("sh:"<<sh);
    cdebug("left:"<<left());

    if(th+bottom()+4<sh)
      setRect(AGRect2(left(),bottom()+2,tw,th));
    else if(top()-4-tw>0)
      // above
      setRect(AGRect2(left(),top()-th-2,tw,th));
    else if(tw+right()+4<sw)
      // right
      setRect(AGRect2(right()+2,top(),tw,th));
    else
      // above
      setRect(AGRect2(left(),top()-th-2,tw,th));

    if(right()>sw)
      setLeft(sw-2-width());
    //  setRect(AGRect2(0,0,tw,th));

    cdebug(getRect());

    //  mFont.setColor(AGColor(0,0,0));
    AGEdit *e=new AGEdit(this,getRect().origin().shrink(1));
    e->setText(mText);
    e->setFont(mFont);
    e->setMutable(false);
    e->setBackground(false);
    addChild(e);
  }

AGTooltip::~AGTooltip() throw()
  {}



void AGTooltip::draw(AGPainter &p)
  {
    //  p.transform(getRect());
    p.fillRect(getRect().origin(),mBorderColor);
    p.fillRect(getRect().origin().shrink(1),mBgColor);
  }
