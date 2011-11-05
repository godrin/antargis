/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_scroller.cc
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


#include "ag_scroller.h"
#include "ag_button.h"
#include "ag_border.h"
#include "ag_background.h"
#include "rk_debug.h"

AGScroller::AGScroller(AGWidget *pParent,const AGRect2 &pRect,bool pHor):
  AGWidget(pParent,pRect),
  sigValueChanged(this,"sigValueChanged"),
  mHorizontal(pHor)
  {
    mBorder=new AGBorder("button.border.normal");
    mBackground=new AGBackground("button.background.normal");

    mMax=1;
    mMin=0;
    mValueWidth=0.3;
    mValue=0.2;
    mStepping=0.1;

    mVirtualValue=mValue;

    mBorderWidth=2;

    if(mHorizontal)
      {
        mB1=new AGButton(this,AGRect2(mBorderWidth,mBorderWidth,height()-mBorderWidth*2,height()-mBorderWidth*2),"");
        mB2=new AGButton(this,AGRect2(width()+mBorderWidth-height(),mBorderWidth,height()-mBorderWidth*2,height()-mBorderWidth*2),"");

        mScroller=new AGButton(this,AGRect2(mBorderWidth+height(),mBorderWidth,(width()-2*height())*0.5  ,height()-mBorderWidth*2),"");

        //      mScroller->setSurface(AGSurface::load("data/gui/v_bars.png"));
      }
    else
      {
        mB1=new AGButton(this,AGRect2(mBorderWidth,mBorderWidth,width()-mBorderWidth*2,width()-mBorderWidth*2),"");
        mB2=new AGButton(this,AGRect2(mBorderWidth,height()+mBorderWidth-width(),width()-mBorderWidth*2,width()-mBorderWidth*2),"");

        mScroller=new AGButton(this,AGRect2(mBorderWidth,mBorderWidth+width(),width()-mBorderWidth*2,(height()-2*width())*0.5),"");

        //      mScroller->setSurface(AGSurface::load("data/gui/h_bars.png"));
      }

    addChild(mB1);
    addChild(mB2);
    addChild(mScroller);

    mScroller->sigDragBy.connect(slot(this,&AGScroller::eventDragBy));
    mB1->sigClick.connect(slot(this,&AGScroller::eventUpClick));
    mB2->sigClick.connect(slot(this,&AGScroller::eventDownClick));

    updateScroller();

    if(mHorizontal)
      {
        mB1->setSurface(AGSurface::load("data/gui/arrow_left.png"));
        mB2->setSurface(AGSurface::load("data/gui/arrow_right.png"));
        mScroller->setSurface(AGSurface::load("data/gui/v_bars.png"));
      }
    else
      {
        mB1->setSurface(AGSurface::load("data/gui/arrow_up.png"));
        mB2->setSurface(AGSurface::load("data/gui/arrow_down.png"));
        mScroller->setSurface(AGSurface::load("data/gui/h_bars.png"));
      }

    mB1->setCaching(false);
    mB2->setCaching(false);
    mScroller->setCaching(false);
    setCaching(true);
  }

AGScroller::~AGScroller() throw()
  {
    checkedDelete(mBorder);
    checkedDelete(mBackground);
  }

void AGScroller::draw(AGPainter &p)
  {
    AGRect2 pr=getRect().origin();
    mBackground->draw(pr,p);
    mBorder->draw(pr,p);
  }


bool AGScroller::eventDragBy(AGEvent *pEvent)
  {
    AGVector2 v=pEvent->getVector();
    if(mHorizontal)
      {
        mVirtualValue+=v[0]/getMovingWidth()*(mMax-mMin);
        mVirtualValue=std::max(mVirtualValue,mMin);
        mVirtualValue=std::min(mVirtualValue,mMax);
        updateScroller();
      }
    else
      {
        mVirtualValue+=v[1]/getMovingWidth()*(mMax-mMin);
        mVirtualValue=std::max(mVirtualValue,mMin);
        mVirtualValue=std::min(mVirtualValue,mMax);
        updateScroller();
      }
    return true;
  }

float AGScroller::getScrollerWidth()
  {
    float v=0;
    if(mHorizontal)
      v=(width()-mBorderWidth*2-getScrollerHeight()*2);
    else
      v=(height()-mBorderWidth*2-getScrollerHeight()*2);

    if(mMax!=mMin)
      v*=mValueWidth/(mMax-mMin);
    return v;

  }
float AGScroller::getScrollerHeight()
  {
    if(mHorizontal)
      return (height()-mBorderWidth*2);
    else
      return (width()-mBorderWidth*2);

  }
float AGScroller::getMovingWidth()
  {
    if(mHorizontal)
      return width()-getScrollerWidth()-2*mBorderWidth-2*getScrollerHeight();
    else
      {
        return height()-getScrollerWidth()-2*mBorderWidth-2*getScrollerHeight();
      }
  }

void AGScroller::updateScroller()
  {
    mVirtualValue=std::max(mVirtualValue,mMin);
    mVirtualValue=std::min(mVirtualValue,mMax);


    float nv=int((mVirtualValue-mMin)/mStepping+0.5)*mStepping+mMin;

    if(nv!=mValue)
      {
        mValue=nv;
        sigValueChanged(new AGEvent(this,"sigValueChanged"));
      }


    if(mHorizontal)
      {
        float x=mBorderWidth+getScrollerHeight();
        if(mMax!=mMin)
          x+=getMovingWidth()*(mValue-mMin)/(mMax-mMin);
        float y=mBorderWidth;
        float w=getScrollerWidth();
        float h=getScrollerHeight();
        mScroller->setRect(AGRect2(x,y,w,h));
      }
    else
      {
        float x=mBorderWidth;
        float y=mBorderWidth+getScrollerHeight();

        if(mMax!=mMin)
          y+=getMovingWidth()*(mValue-mMin)/(mMax-mMin);
        float h=getScrollerWidth();
        float w=getScrollerHeight();

        mScroller->setRect(AGRect2(x,y,w,h));
      }
    queryRedraw();
  }

bool AGScroller::eventUpClick(AGEvent *pEvent)
  {
    mVirtualValue-=mStepping;
    mVirtualValue=std::max(mVirtualValue,mMin);
    mVirtualValue=std::min(mVirtualValue,mMax);
    updateScroller();
    return true;
  }
bool AGScroller::eventDownClick(AGEvent *pEvent)
  {
    mVirtualValue+=mStepping;
    mVirtualValue=std::max(mVirtualValue,mMin);
    mVirtualValue=std::min(mVirtualValue,mMax);
    updateScroller();
    return true;
  }

void AGScroller::setValue(float pValue)
  {
    mVirtualValue=pValue;
    updateScroller();
  }
void AGScroller::setScrollerSize(float pSize)
  {
    mValueWidth=pSize;
    updateScroller();
  }
void AGScroller::setInterval(float pMin,float pMax)
  {
    mMin=pMin;
    mMax=pMax;
    assert(mMin<=mMax);
    updateScroller();
  }
void AGScroller::setStepping(float pStep)
  {
    mStepping=pStep;
    updateScroller();
  }


float AGScroller::getValue() const
{
  return mValue;
}
float AGScroller::getMin() const
{
  return mMin;
}
float AGScroller::getMax() const
{
  return mMax;
}
