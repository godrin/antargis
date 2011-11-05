/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_listbox.cc
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

#include "rk_debug.h"
#include "ag_edit.h"
#include "ag_listbox.h"
#include "ag_theme.h"
#include "ag_scroller.h"
#include "ag_stringstream.h"

#include <sstream>

AGListBoxItem::AGListBoxItem(AGString pID,AGStringUtf8 pValue)
  {
    id=pID;
    value=pValue;
  }

AGListBox::AGListBox(AGWidget *pParent,const AGRect2 &pRect):AGWidget(pParent,pRect),
  sigSelect(this,"sigSelect"),
  sigDoubleClick(this,"sigDoubleClick")
{
  // insert AGEdits
  int y=0;
  int count=0;
  mItemHeight=getTheme()->getInt("listbox.item.height");
  if(mItemHeight<5)
    mItemHeight=25;

  mScroller=new AGScroller(this,AGRect2(width()-mItemHeight,0,mItemHeight,height()),false);
  addChild(mScroller);

  mScroller->sigValueChanged.connect(slot(this,&AGListBox::eventScroller));

  for(;y<pRect.h();y+=mItemHeight,count++)
    {
      AGRect2 r(0,y,pRect.w()-mItemHeight,mItemHeight);
      AGEdit *e=new AGEdit(this,r);
      e->setMutable(false);
      e->setBackground(false);
      //e->setFont(f);
      AGStringStream os;
      os<<"ListBoxItem"<<count;
      e->setName(os.str());

      mEdits.push_back(e);
      addChild(e);
    }
  mHeight=count;
  mY=0;
  mSelected=-1;

  setTheme("");
}

AGListBox::~AGListBox() throw()
  {}


void AGListBox::setTheme(const AGString &pTheme)
  {
    AGString themeName=pTheme;
    if(themeName.length()==0)
      themeName="listbox";
    else
      themeName+="listbox";

    AGLocalTheme theme=getTheme()->getTheme(themeName);
    mBackground=AGBackground(theme,"background");
    mHilight=AGBackground(theme,"selected");

    AGFont f=theme.getFont("font");

    // change fonts for edits
    for(std::vector<AGEdit*>::iterator i=mEdits.begin();i!=mEdits.end();i++)
      (*i)->setFont(f);

    rearrange();


  }

void AGListBox::rearrange()
  {
    mItemHeight=getTheme()->getInt("listbox.item.height");
    if(mItemHeight<5)
      mItemHeight=25;

    mScroller->setRect(AGRect2(width()-mItemHeight,0,mItemHeight,height()));

    int count=0;
    for(int y=0;y<height();y+=mItemHeight,count++)
        mEdits[count]->setRect(AGRect2(0,y,width()-mItemHeight,mItemHeight));
  }

void AGListBox::insertItem(AGString pID,AGStringUtf8 pValue)
  {
    mItems.push_back(AGListBoxItem(pID,pValue));
    arrange();
  }
void AGListBox::selectItem(AGString pID)
  {
    for(size_t i=0;i<mItems.size();i++)
      if(mItems[i].id==pID)
        {
          mSelected=i;
          mY=std::max(0,int(i)-mHeight/2); // FIXME: maybe find better zoom
        }
    arrange();
  }

AGString AGListBox::getSelectedID() const
{
  if(mSelected>=0 && size_t(mSelected)<mItems.size())
    return mItems[mSelected].id;
  return "";
}

AGString AGListBox::getSelectedValue() const
{
  if(mSelected>=0 && size_t(mSelected)<mItems.size())
    return mItems[mSelected].id;
  return "";
}

bool AGListBox::eventKeyDown(AGEvent *m)
  {
    if(m->isSDLEvent())
      {
        SDLKey k=m->getKey();
        if(k==SDLK_UP)
          {
            if(mSelected<2 && mItems.size()>0)
              mSelected=0;
            else
              mSelected--;
            if(mSelected<mY)
              mY=mSelected;
            arrange();
            return true;
          }
        else if(k==SDLK_DOWN)
          {
            if(mSelected<int(mItems.size())-1)
              mSelected++;
            if(mSelected>=mY+mHeight)
              mY++;
            arrange();
            return true;
          }
      }
    return AGWidget::eventKeyDown(m);
  }


void AGListBox::arrange()
  {
    size_t y,y2=0;

    // first clear all
    for(y=0;y<mEdits.size();y++)
      mEdits[y]->setText("");

    for(y=mY,y2=0;y<(size_t)(mY+mHeight) && y<mItems.size();y++,y2++)
      mEdits[y2]->setText(mItems[y].value);

    updateScroller();
  }

void AGListBox::draw(AGPainter &p)
  {
    mBackground.draw(getRect().origin(),p);
    p.pushMatrix();

    if(mSelected>=0 && mSelected<(int)mItems.size())
      {
        int y=mSelected-mY;

        AGRect2 r(0,y*mItemHeight,width(),mItemHeight);
        p.transform(r);
        mHilight.draw(getRect().origin(),p);
      }
    p.popMatrix();
    AGWidget::draw(p);
  }

bool AGListBox::eventMouseClick(AGEvent *e)
  {
    CTRACE;
    if(e->isSDLEvent())
      {
        AGVector2 p=e->getMousePosition();

        int b=e->getButton();
        //      cdebug(b);
        if(b==4)
          {
            // up wheel
            if(mY>0)
              mY--;
            arrange();
            return true;
          }
        else if(b==5)
          {
            // down wheel
            if(mY<=(int)mItems.size()-mHeight)
              mY++;
            arrange();
            return true;
          }
        else
          {

            int y=(int)(p[1]-getScreenRect().y());
            y/=mItemHeight;
            int n=y+mY;
            if(n<(int)mItems.size())
              mSelected=n;

            e->setName("sigSelect");

            sigSelect(e);
            return true;
          }
      }
    return false;
  }

AGListBox &toAGListBox(AGWidget &w)
  {
    return dynamic_cast<AGListBox&>(w);
  }

void AGListBox::clearList()
  {
    mY=mSelected=0;
    mItems.clear();
  }

bool AGListBox::eventMouseButtonDown(AGEvent *m)
  {
    // ensure that event is eaten
    bool r=AGWidget::eventMouseButtonDown(m);
    if(m->isSDLEvent())
      if(getScreenRect().contains(m->getMousePosition()))
        return true;
    return r;
  }
bool AGListBox::eventMouseButtonUp(AGEvent *m)
  {
    // ensure that event is eaten
    bool r=AGWidget::eventMouseButtonUp(m);
    if(m->isSDLEvent())
      if(getScreenRect().contains(m->getMousePosition()))
        return true;
    return r;
  }

bool AGListBox::eventScroller(AGEvent *e)
  {
    mY=(int)mScroller->getValue();
    arrange();
    return false;
  }

void AGListBox::updateScroller()
  {
    // update scroller
      {
        int itemCount=mItems.size();
        int visibleCount=(int)(height()/mItemHeight);

        int maxVal=std::max(0,itemCount-visibleCount);

        /*    cdebug("maxVal:"<<maxVal);
    cdebug("vis:"<<visibleCount);
    cdebug("items:"<<itemCount);
    cdebug("my:"<<mY);
         */
        mScroller->setInterval(0,maxVal);
        mScroller->setScrollerSize(std::min(visibleCount,maxVal));
        mScroller->setValue(mY);
        mScroller->setStepping(1);
      }

  }

std::map<AGString,AGStringUtf8> AGListBox::getValues() const
{
  std::map<AGString,AGStringUtf8> m;
  for(std::vector<AGListBoxItem>::const_iterator i=mItems.begin();i!=mItems.end();i++)
    {
      m.insert(std::make_pair(i->id,i->value));
    }
  return m;
}
