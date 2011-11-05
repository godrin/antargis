/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_colorbutton.cc
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

#include "ag_colorbutton.h"
#include "ag_layoutfactory.h"

AGColorButton::AGColorButton(AGWidget *pParent,const AGRect2 &r,int x,int y):
  AGWidget(pParent,r),gridx(x),gridy(y),mColors(x*y)
  {
  }

AGColorButton::~AGColorButton() throw()
  {}


void AGColorButton::draw(AGPainter &p)
  {
    AGWidget::draw(p);

    int i=0;
    AGRect2 ar(getRect().origin());
    float w=ar.w()/gridx;
    float h=ar.h()/gridy;
    for(int y=0;y<gridy;y++)
      for(int x=0;x<gridx;x++)
        {
          AGRect2 r(ar.w()*x/gridx,ar.h()*y/gridy,w,h);
          p.fillRect(r,mColors[i++]);
        }
  }

AGColor AGColorButton::getColor() const
{
  return mColor;
}

void AGColorButton::setColor(const AGColor &c)
  {
    mColor=c;
  }
void AGColorButton::setColor(int x,int y,const AGColor &c)
  {
    assert(x>=0);
    assert(y>=0);
    assert(x<gridx);
    assert(y<gridy);
    mColors[x+y*gridx]=c;
  }


AGColorButton &toAGColorButton(AGWidget &pWidget)
  {
    return dynamic_cast<AGColorButton&>(pWidget);
  }

bool AGColorButton::eventMouseClick(AGEvent *e)
  {
    //  const AGSDLEvent *e=dynamic_cast<const AGSDLEvent*>(m);
    if(e->isSDLEvent())
      {
        AGVector2 p=e->getMousePosition();
        p-=getScreenRect().getV0();
        float gx=p[0]*gridx/getRect().w();
        float gy=p[1]*gridy/getRect().h();
        float d=gx+gy*gridx;
        cdebug(p[0]<<"   "<<p[1]);
        cdebug("gx:"<<gx<<"  gy:"<<gy);
        assert(d<gridx*gridy && d>=0);
        mColor=mColors[size_t(d)];
        cdebug(mColor.toString());
      }

    return AGWidget::eventMouseClick(e);
  }


