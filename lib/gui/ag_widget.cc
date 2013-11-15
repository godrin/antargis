/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_widget.cc
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

#include "ag_widget.h"
#include "ag_screen.h"
#include "ag_main.h"
#include "ag_menu.h"
#include "ag_kill.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include "ag_tooltip.h"
#include "ag_application.h"
#include "ag_layout.h"
#include "ag_config.h"
#include "ag_clip.h"
#include "ag_painter.h"
#include "math_algebra.h" // FIXME: remove this ASAP


#define FOCUS_BY_SORT


using namespace std;

static bool gNewClippingTechnique=true;

void setNewClippingTechnique(bool f)
{
  gNewClippingTechnique=f;
}
bool getNewClippingTechnique()
{
  return gNewClippingTechnique;
}

std::set<AGWidget*> AGWidget::allWidgets;


AGWidget::AGWidget(AGWidget *pParent,const AGRect2 &r):
  sigMouseEnter(this,"sigMouseEnter"),
  sigMouseLeave(this,"sigMouseLeave"),
  sigClick(this,"sigClick"),
  sigDragBy(this,"sigDragBy"),
  mApp(0),
  mRect(r),mClientWorld(r.origin()),mUseClientRect(false),
  mParent(pParent),mChildrenEventFirst(false),mChildrenDrawFirst(false),mMouseIn(false),mButtonDown(false),
  mFixedWidth(false),mFixedHeight(false),mVisible(true),mCaching(false),
  mHasFocus(false),mFocus(0),mAlpha(-1)

{
  allWidgets.insert(this);

  mEventsInited=false;
  if(mParent)
    mParent->addChildRef(this);

  mChangeRect=AGRect2(0,0,0,0);
  mCache=0;
  mCacheTouched=false;
  mTooltipWidget=0;
  mModal=false;
}

AGWidget::~AGWidget() throw()
{
  allWidgets.erase(this);

  std::list<AGWidget*>::iterator i=mChildren.begin();
  for(;i!=mChildren.end();i++)
  {
    if(valid(*i))
    {
      (*i)->setParent(0);
    }
  }
  for(std::set<AGWidget*>::iterator i=mRefChildren.begin();i!=mRefChildren.end();i++)
  {
    if(valid(*i))
    {
      (*i)->setParent(0);
    }
  }

  if(getParent())
  {
    if(valid(getParent()))
    {
      getParent()->eventChildrenDeleted(this);
    }
  }
}

bool AGWidget::valid(AGWidget *pWidget)
{
  return (allWidgets.find(pWidget)!=allWidgets.end());

}


std::list<AGWidget*> AGWidget::getChildren()
{
  return mChildren;
}


void AGWidget::removeChild(AGWidget *w)
{
  std::list<AGWidget*>::iterator i=std::find(mChildren.begin(),mChildren.end(),w);
  if(i!=mChildren.end())
  {
    mChildren.erase(i);
    w->setParent(0);
  }
}


void AGWidget::eventChildrenDeleted(AGWidget *pWidget)
{
  std::list<AGWidget*>::iterator i=mChildren.begin();
  for(;i!=mChildren.end();i++)
  {
    if(*i==pWidget)
    {
      mChildren.erase(i);
      //break; // do not break, beacuse a list could have this child more than once ???
    }
  }
  i=mToClear.begin();
  for(;i!=mToClear.end();i++)
  {
    if(*i==pWidget)
    {
      mToClear.erase(i);
      // break; // same as above
    }
  }
  mRefChildren.erase(pWidget);
}


void AGWidget::draw(AGPainter &p)
{
}

void AGWidget::delObjects()
{
  // clearing widgets
  if(mToClear.size())
  {
    std::list<AGWidget*>::iterator i=mToClear.begin();
    for(;i!=mToClear.end();i++)
    {
      (*i)->setParent(0); // lets play it safe
      checkedDelete(*i);
    }
    mToClear.clear();
  }
}

void AGWidget::drawAll(AGPainter &p)
{
  delObjects();

  if(!mVisible)
    return;

  if(mCache)
  {
    if(mAlpha>0 && mAlpha<1) {
      p.blit(*mCache,getRect(),getRect().origin(),AGColor(255,255,255,(int)(mAlpha*255)));
    } else
      p.blit(*mCache,getRect(),getRect().origin());
  }
  else
  {
    p.pushMatrix();

    p.clip(getRect());
    //p.transform(getRect());
    p.transform(innerToOuter().getMatrix());

    if(!mChildrenDrawFirst)
      draw(p);

    std::list<AGWidget*>::reverse_iterator i=mChildren.rbegin(); // draw from back to front

    for(;i!=mChildren.rend();i++)
      drawChild(p,*i);

    if(mChildrenDrawFirst)
      draw(p);

    drawAfter(p);

    p.popMatrix();
    setDrawn();
  }
}

void AGWidget::drawChild(AGPainter &p,AGWidget *pWidget)
{
  pWidget->drawAll(p);
}

AGProjection2D AGWidget::getClientProjection() const
{
  return mClientProj;
}

AGRect2 AGWidget::getClientWorld() const
{
  return mClientWorld;
}


AGRect2 AGWidget::getRect() const
{
  return mRect;
}

AGRect2 AGWidget::getClientRect() const
{
  if(mUseClientRect)
    return mClientWorld;
  return mRect.origin();
}

void AGWidget::setClient(const AGRect2 &pWorld,const AGProjection2D &pProjection)
{
  mClientWorld=pWorld;
  mClientProj=pProjection;
  assert(isInvertable(mClientProj.getMatrix()));
  mUseClientRect=true;
}


bool AGWidget::processEvent(AGEvent *event)
{
  if(!mVisible)
    return false;

  bool processed = false;
  // do i have a capturehook set ? (modal)
  // i will send that event to my children

  std::list<AGWidget*>::iterator i;

  std::list<AGWidget*> children=mChildren; // copy children, so that changes do not affect iteration
  for(i=children.begin();i!=children.end() && !processed; i++)
    processed=letChildProcess(*i,event);

  if(processed)
    return processed;

  event->setCaller(this);

  // let me see if i can process it myself
  if(AGMessageObject::processEvent(event))
    return true;

  checkFocus();
  if(mModal)
    return true;
  return false;
}

bool AGWidget::letChildProcess(AGWidget *pChild,AGEvent *event)
{
  bool retValue;
  AGVector2 old=event->getRelMousePosition();
  AGVector2 newP=outerToInner().project(old);
  /*
     if(mUseClientRect)
     newP=mClientProj.inverse().project(old-mRect.getV0()); //FIXME - mRect must influence this, too (????)
     else
     newP=old-getRect().getV0();
     */
  event->setRelMousePosition(newP);
  bool wasClipped=event->isClipped();

  event->setClipped(wasClipped || (!getRect().contains(old)));

  retValue=pChild->processEvent(event);

  event->setClipped(wasClipped);
  event->setRelMousePosition(old);

  return retValue;
}


bool AGWidget::eventShow()
{
  return false;
}
bool AGWidget::eventHide()
{
  return false;
}

bool AGWidget::eventMouseEnter()
{
  if(mTooltip.length())
  {
    mTooltipWidget=new AGTooltip(getScreenRect(),mTooltip);
    getApp()->setTooltip(mTooltipWidget);
  }

  return false;
}
bool AGWidget::eventMouseLeave()
{
  if(mTooltipWidget)
  {
    getApp()->resetTooltip(mTooltipWidget);
    mTooltipWidget=0;
  }
  return false;
}

bool AGWidget::hovered() const
{
  return mMouseIn;
}


bool AGWidget::eventMouseMotion(AGEvent *e)
{
  if(!visible())
    return false;

  if(e->isSDLEvent())
  {
    //cdebug(e->isFocusTaken()<<"::"<<getName());
    if(getRect().contains(e->getRelMousePosition()) && !e->isClipped())
    {
      if(!mMouseIn)
      {
        mMouseIn=true;
        eventMouseEnter() || sigMouseEnter(e);
      }
    }
    else
    {
      if(mMouseIn)
      {
        mMouseIn=false;
        eventMouseLeave() || sigMouseLeave(e);
      }
    }
    if(mButtonDown)
    {
      AGVector2 v=e->getMousePosition()-mOldMousePos;
      e->setVector(v);
      eventDragBy(e,v) || sigDragBy(e);
      mOldMousePos=e->getMousePosition();
    }
  }
  return false;
}

bool AGWidget::eventMouseButtonDown(AGEvent *e)
{
  if(e->isSDLEvent())
  {
    if(getRect().contains(e->getRelMousePosition()))
    {
      mButtonDown=true;
      mOldMousePos=e->getMousePosition();
      return false;
    }
  }
  return false;
}

bool AGWidget::eventMouseButtonUp(AGEvent *e)
{
  bool was=mButtonDown;

  mButtonDown=false;

  if(e->isSDLEvent())
  {
    if(getRect().contains(e->getRelMousePosition()))
    {
      if(was)
      {
        e->setName("sigClick");
        AGApplication *app=getApp();
        assert(app);
        if(app)
        {
          AGWidget *overlay=getApp()->getOverlay();

          if(!isParent(overlay))
            app->setOverlay(0);
        }
        if(!isParent(getApp()->getOverlay()))
          getApp()->setOverlay(0);

        if(canFocus())
          gainFocus();

        return doClick(e);

      }
    }
  }
  return false;
}

bool AGWidget::eventMouseButtonDownClipped(AGEvent *pEvent,const AGVector2 &pPosition)
{
  AGProjection2D inv=mClientProj.inverse();
  AGVector2 cPos=inv.project(pPosition);
  for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
  {
    if(containsPoint(*i,pPosition))
    {
      if((*i)->eventMouseButtonDownClipped(pEvent,cPos))
        return true;
    }
  }
  return false;
}
bool AGWidget::eventMouseButtonUpClipped(AGEvent *pEvent,const AGVector2 &pPosition)
{
  return false;
}
bool AGWidget::eventMouseMotionClipped(AGEvent *pEvent,const AGVector2 &pPosition)
{
  return false;
}


bool AGWidget::doClick(AGEvent *e)
{
  return (eventMouseClick(e) || sigClick(e) || (!sigClickBoost.empty() && sigClickBoost(this)));
}

bool AGWidget::eventMouseClick(AGEvent *m)
{
  return false;
}

void AGWidget::addChildRef(AGWidget *pWidget)
{
  assert(pWidget);
  mRefChildren.insert(pWidget);
}


void AGWidget::addChild(AGWidget *w)
{
  if(w->getParent())
    w->getParent()->mRefChildren.erase(w);
  mRefChildren.erase(w);

  mChildren.push_front(w); // set on top
  if(mHasFocus && w->canFocus())
  {
    gainFocus(w);
  }
  if(!w->getParent())
    w->setParent(this);
  queryRedraw();
}

void AGWidget::clear() throw()
{
  // delay it till be draw everything - so this doesn't kill widgets while processing events
  std::copy(mChildren.begin(),mChildren.end(),std::back_inserter(mToClear));
  mChildren.clear();
}

void AGWidget::erase(AGWidget *w)
{
  std::list<AGWidget*>::iterator i=std::find(mChildren.begin(),mChildren.end(),w);
  if(i!=mChildren.end())
  {
    mToClear.push_back(w);

    mChildren.erase(i);
  }
}

void AGWidget::addChildBack(AGWidget *w)
{
  if(w->getParent())
    w->getParent()->mRefChildren.erase(w);
  mRefChildren.erase(w);

  // check if children already exists
  Children::iterator i;
  do
  {
    i=std::find(mChildren.begin(),mChildren.end(),w);
    if(i!=mChildren.end())
      mChildren.erase(i);
  }while(i!=mChildren.end());


  mChildren.push_back(w); // set on top

  if(mHasFocus && w->canFocus())
  {
    gainFocus(w);
  }
  if(!w->getParent())
    w->setParent(this);
  queryRedraw();
}

void AGWidget::regChange()
{
  AGRect2 t=getScreenRect().grow(5);

  pushChangeRect(t);

  if(mChangeRect.width()==0 || mChangeRect.height()==0)
    mChangeRect=t;
  else
  {
    mChangeRect.include(t[0]);
    mChangeRect.include(t[1]);
  }
}

void AGWidget::setRect(const AGRect2 &pRect)
{
  if(mCache&&(width()!=pRect.width()||height()!=pRect.height()))
    setCaching(true);

  regChange();
  mRect=pRect;
  regChange();
  if(mParent)
    mParent->redraw();

}

float AGWidget::minWidth() const
{
  // accumulate max
  std::list<AGWidget*>::const_iterator i=mChildren.begin();
  float m=0;

  for(;i!=mChildren.end();i++)
    m=std::max(m,(*i)->minWidth());

  return m;
}

float AGWidget::minHeight() const
{
  // accumulate max
  std::list<AGWidget*>::const_iterator i=mChildren.begin();
  float m=0;

  for(;i!=mChildren.end();i++)
    m=std::max(m,(*i)->minHeight());

  return m;
}

float AGWidget::width() const
{
  return mRect.w();
}

float AGWidget::height() const
{
  return mRect.h();
}

bool AGWidget::fixedWidth() const
{
  return mFixedWidth;
}

bool AGWidget::fixedHeight() const
{
  return mFixedHeight;
}

void AGWidget::setWidth(float w)
{
  bool changed=(width()!=w);
  regChange();
  mRect.setWidth(w);
  if(mCache && changed)
    setCaching(true);
  regChange();
  queryRedraw();
}
void AGWidget::setHeight(float h)
{
  bool changed=(height()!=h);
  regChange();
  mRect.setHeight(h);
  if(mCache && changed)
    setCaching(true);
  regChange();
  queryRedraw();
}

void AGWidget::setTop(float y)
{
  regChange();
  mRect.setTop(y);
  regChange();
}
void AGWidget::setLeft(float x)
{
  regChange();
  mRect.setLeft(x);
  regChange();
}
float AGWidget::bottom() const
{
  return mRect[1][1];
}
float AGWidget::right() const
{
  return mRect[1][0];
}


float AGWidget::top() const
{
  return mRect.y();
}

float AGWidget::left() const
{
  return mRect.x();
}

void AGWidget::show()
{
  mVisible=true;
  queryRedraw();
}
void AGWidget::hide()
{
  mVisible=false;
  queryRedraw();
}

void AGWidget::setVisible(bool v)
{
  if(mVisible!=v)
  {
    queryRedraw();
  }
  mVisible=v;
}


void AGWidget::setParent(AGWidget *pParent)
{
  if(!mParent)
  {
    AGWidget *old=mParent;
    mParent=pParent;
    if(mParent==0 && old!=0)
      old->eventChildrenDeleted(this);
  }

  mParent=pParent;
}


AGWidget *AGWidget::getParent()
{
  return mParent;
}

AGVector2 AGWidget::getScreenPosition() const
{
  return getScreenRect().getV0();
}

/**
 * returns the extends of this widget wrt. to the screen-coordinates
 */
AGRect2 AGWidget::getScreenRect() const
{
  AGRect2 r=getRect();
  if(mParent)
  {
    AGRect2 result=mParent->toScreen(getRect());
    return result;
  }
  return r;
}

AGRect2 AGWidget::toScreen(const AGRect2&p) const
{
  AGRect2 r=innerToOuter(p);
  if(mParent)
    r=mParent->toScreen(r);
  return r;
}

AGVector2 AGWidget::toScreen(const AGVector2&p) const
{
  AGVector2 r=innerToOuter(p);
  if(mParent)
    r=mParent->toScreen(r);
  return r;
}


AGRect2 AGWidget::fromScreen(const AGRect2 &p) const
{
  AGRect2 r=outerToInner(p);
  if(mParent)
    r=mParent->toScreen(r);
  return r;
}



AGVector2 AGWidget::fromScreen(const AGVector2 &p) const
{
  AGVector2 r=outerToInner(p);
  if(mParent)
    r=mParent->toScreen(r);
  return r;
  /*  if(!mParent)
      return p;
      AGRect2 r(mParent->getScreenRect());
      return p-r[0];*/
}

bool AGWidget::canFocus() const
{
  std::list<AGWidget*>::const_iterator i=mChildren.begin();

  for(;i!=mChildren.end();i++)
    if((*i)->canFocus())
    {

      return true;
    }

  return false;
}

bool AGWidget::eventGotFocus()
{
  mHasFocus=true;
  return false;
}

bool AGWidget::eventLostFocus()
{
  if(mFocus)
    mFocus->eventLostFocus();
  mHasFocus=false;
  mFocus=0;

  if(mChildren.size()>0)
    (*mChildren.begin())->eventLostFocus();

  return false;
}

void AGWidget::gainCompleteFocus(AGWidget *pWidget)
{
#ifdef FOCUS_BY_SORT
  if(mParent)
    mParent->gainCompleteFocus(this);
  if(pWidget)
  {
    std::list<AGWidget*>::iterator i=std::find(mChildren.begin(),mChildren.end(),pWidget);
    if(i!=mChildren.end())
    {
      mChildren.erase(i);
      mChildren.push_front(pWidget);
    }
  }
#endif
}

void AGWidget::gainFocus(AGWidget *pWidget)
{
#ifdef FOCUS_BY_SORT
  if(pWidget)
  {
    std::list<AGWidget*>::iterator i=std::find(mChildren.begin(),mChildren.end(),pWidget);
    if(i!=mChildren.end())
    {
      mChildren.erase(i);

      if(mChildren.size()>0)
        (*mChildren.begin())->eventLostFocus();

      mChildren.push_front(pWidget);
      pWidget->eventGotFocus();

    }
  }
  else if(mParent)
  {
    if(canFocus())
    {
      mParent->gainFocus(this);
    }
  }
#else
  if(pWidget==0 && mParent)
    mParent->gainFocus(this);
  else if(mParent)
    mParent->gainFocus(pWidget);
  else
    gainFocusDown(pWidget);
#endif
}

void AGWidget::gainFocusDown(AGWidget *pWidget)
{
  std::list<AGWidget*>::iterator i;
  i=std::find(mChildren.begin(),mChildren.end(),pWidget);
  if(i!=mChildren.end())
  {
    // found
    if(!mHasFocus)
    {
      if(mParent)
        mParent->gainFocus(this);
      else
      {
        mHasFocus=true;
        eventGotFocus();
      }
    }

    if(mFocus!=pWidget)
    {
      if(mFocus)
        mFocus->eventLostFocus();
      mFocus=pWidget;
      mFocus->eventGotFocus();
    }
  }
  else
  {
    for(i=mChildren.begin();i!=mChildren.end();i++)
      (*i)->gainFocusDown(pWidget);
  }
}

void AGWidget::checkFocus()
{
  if(mChildren.size()>0 && mFocus && mHasFocus)
  {
    if(mFocus!=*mChildren.begin())
    {
      std::list<AGWidget*>::iterator i;

      i=std::find(mChildren.begin(),mChildren.end(),mFocus);
      // delete children and set to front
      mChildren.erase(i);
      mChildren.push_front(mFocus);
    }
  }
}

bool AGWidget::hasFocus(const AGWidget *pWidget)
{
#ifdef FOCUS_BY_SORT
  if(pWidget==0)
  {
    if(mParent)
      return mParent->hasFocus(this);
    else
      return true;
  }
  if(mChildren.size()==0)
    return true; // some error

  if(*mChildren.begin()==pWidget)
  {
    if(mParent)
      return mParent->hasFocus(this); // ok - so go on and check if "this" has focus
    return true; // ok
  }

  return false;
#else
  if(pWidget==0)
  {
    if(mParent)
      return mParent->hasFocus(this);
    else
      return true;
  }
  else if(mFocus!=pWidget)
    return false;
  else if(mParent)
    return mParent->hasFocus(this);
  return true;
#endif
}


bool AGWidget::eventDragBy(AGEvent *event,const AGVector2 &pDiff)
{
  return false;
}

bool AGWidget::getFocus() const
{
  return mHasFocus;
}

const AGString &AGWidget::getName() const
{
  return mName;
}
void AGWidget::setName(const AGString &pName)
{
  mName=pName;
}

void AGWidget::drawAfter(AGPainter &p)
{
}

bool AGWidget::visible() const
{
  return mVisible;
}

/// override this and return true, if widget is opaque and you want to increase performance
bool AGWidget::isOpaque() const
{
  return false;
}



AGWidget *AGWidget::getChild(const AGString &pName)
{
  if(mName==pName)
    return this;


  AGWidget *w=0;
  std::list<AGWidget*>::iterator i=mChildren.begin();

  for(;i!=mChildren.end();i++)
  {
    w=(*i)->getChild(pName);
    if(w)
      break;
  }
  return w;
}

void AGWidget::setModal(bool pModal)
{
  mModal=pModal;
}


AGWidget *toAGWidget(AGMessageObject *o)
{
  return dynamic_cast<AGWidget*>(o);
}


bool AGWidget::redraw() const
{
  return mCacheTouched;
}

bool AGWidget::checkRedraw() const
{
  if(!mVisible)
    return false;
  if(redraw())
    return true;
  for(std::list<AGWidget*>::const_iterator i=mChildren.begin();i!=mChildren.end();++i)
    if((*i)->checkRedraw())
      return true;
  return false;
}

void AGWidget::prepareDrawAll()
{
  if(!mVisible)
    return;

  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();++i)
    (*i)->prepareDrawAll();

  prepareDraw();

}


void AGWidget::prepareDraw()
{
  if(!mVisible)
    return;

  if(mCache)
  {
    if(checkRedraw() || !mCache->hasTexture())
    {
      mCache->clearContent();

      AGPainter p(*mCache);

      if(!mChildrenDrawFirst)
        draw(p);

      std::list<AGWidget*>::reverse_iterator i=mChildren.rbegin(); // draw from back to front

      for(;i!=mChildren.rend();i++)
        (*i)->drawAll(p);

      if(mChildrenDrawFirst)
        draw(p);

      drawAfter(p);

      setDrawn();

      if(mParent)
      {
        mParent->queryRedraw();
      }

      assert(checkRedraw()==false);
      assert(mCache->hasTexture());
    }
  }
}

/**
 * enable texture caching for this widget (widgetTextureCache in config must be enabled for this)
 * cache will be discarded whenever you modify the widget (e.g. by setting its text or the like).
 * You'll have to call queryRedraw in subclasses. This works for both gl- and sdl-mode.
 */
void AGWidget::setCaching(bool pEnable)
{

  if(getConfig()->get("widgetTextureCache")=="false")
    return;
  getConfig()->set("widgetTextureCache","true");

  mCaching=pEnable;
  checkedDelete(mCache);

  mCache=0;
  mCacheTouched=false;
  if(pEnable)
  {
    mCache=new AGTexture((int)width(),(int)height());

    mCacheTouched=true;
  }
}

void AGWidget::setDrawn()
{
  if(!mVisible)
    return;

  mCacheTouched=false;
  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();++i)
    (*i)->setDrawn();
  mChangeRect=AGRect2(0,0,0,0);
}

void AGWidget::queryRedraw()
{
  //  cdebug(getName()<<"::"<<typeid(*this).name());
  mCacheTouched=true;
  regChange();
}

/**
  \brief special function for "using textures"

  within a texture-garbage-collection run all unused textures get cleaned,
  so this function "uses" the textures. This way they won't get collected
  */
void AGWidget::useTextures()
{
}

/**
  \brief special function for "using textures"

  within a texture-garbage-collection run all unused textures get cleaned,
  so this function "uses" the textures. This way they won't get collected.

  This function calles useTextures() recursively. Don't override this one!
  */
void AGWidget::useTexturesRecursive()
{
  useTextures();
  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();++i)
    (*i)->useTexturesRecursive();
}

AGRect2 AGWidget::getChangeRect()
{
  AGRect2 r=mChangeRect;

  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();++i)
  {
    AGRect2 n=(*i)->getChangeRect();
    if(n.width()!=0 && n.height()!=0)
    {
      if(r.width()!=0 && r.height()!=0)
      {
        r.include(n[0]);
        r.include(n[1]);
      }
      else
        r=n;
    }
  }
  return r;
}

void AGWidget::setTooltip(const AGStringUtf8 &pTooltip)
{
  mTooltip=pTooltip;
}
const AGStringUtf8 &AGWidget::getTooltip() const {
  return mTooltip;
}

AGLayout *AGWidget::getLayout()
{
  AGLayout *l=dynamic_cast<AGLayout*>(this);
  if(l)
    return l;
  if(mParent)
    return mParent->getLayout();
  return 0;
}

void AGWidget::initHandlers()
{
}

void AGWidget::eventTick(float pTime)
{
}
void AGWidget::sigTick(float pTime)
{
  eventTick(pTime);
  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();++i)
    (*i)->sigTick(pTime);
}

void AGWidget::close()
{
  if(mParent)
  {
    mParent->removeChild(this);
    mParent=0;
  }
}

bool AGWidget::isParent(AGWidget *pParent)
{
  if(mParent==pParent)
    return true;
  else if(mParent!=0)
    return mParent->isParent(pParent);
  return false;
}

void AGWidget::acquireClipping(AGClipping &p)
{
  if(!visible())
    return;

  //  cdebug("clipping before:"<<p.toString());
  if(isOpaque())
    p.exclude(getScreenRect());

  for(std::list<AGRect2>::iterator i=mMyChanges.begin();i!=mMyChanges.end();i++)
    p.include(*i);//+getScreenPosition());

  //  cdebug("clipping in:"<<p.toString());

  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();i++)
    (*i)->acquireClipping(p);

  //  cdebug("clipping after:"<<p.toString());
  return;
}


std::list<AGRect2> AGWidget::aquireChanges()
{
  std::list<AGRect2> l;

  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();i++)
  {
    std::list<AGRect2> t=(*i)->aquireChanges();
    // FIXME: check, if opaque and truncates rectangles
    std::copy(t.begin(),t.end(),std::back_inserter(l));
  }
  std::copy(mMyChanges.begin(),mMyChanges.end(),std::back_inserter(l));

  return l;
}
void AGWidget::pushChangeRect(const AGRect2 &pRect)
{
  //  cdebug("push:"<<pRect);
  mMyChanges.push_back(pRect);
}
void AGWidget::clearChangeRects()
{
  //  cdebug("clearing - size was:"<<mMyChanges.size());
  mMyChanges.clear();
  for(std::list<AGWidget*>::iterator i=mChildren.begin();i!=mChildren.end();i++)
    (*i)->clearChangeRects();
}

AGApplication *AGWidget::getApp()
{
  if(!mApp)
    if(mParent)
      return mParent->getApp();
  return mApp;
}

void AGWidget::setApp(AGApplication *pApp)
{
  assert((!mApp)||mApp==pApp||(pApp==0));
  mApp=pApp;
}


AGRect2 AGWidget::getChildRect(const AGRect2 &pRect) const
{
  if(mUseClientRect)
    return mClientProj.project(pRect);
  else
    return pRect;
}
bool AGWidget::containsPoint(AGWidget *pWidget,const AGVector2 &pVector) const
{
  return getChildRect(pWidget->getRect()).contains(pVector);
}

void AGWidget::setButtonDown(bool value,const AGVector2 &startVector)
{
  mOldMousePos=startVector;
  mButtonDown=value;
}

AGProjection2D AGWidget::innerToOuter() const
{
  AGRect2 u(0,0,1,1);
  return AGProjection2D(u,innerToOuter(u));
}
AGProjection2D AGWidget::outerToInner() const
{
  AGRect2 u(0,0,1,1);
  return AGProjection2D(u,outerToInner(u));
}

AGRect2 AGWidget::innerToOuter(const AGRect2 &p) const
{
  AGRect2 m=p;

  if(mUseClientRect)
    m=mClientProj.project(m);
  return m+getRect().getV0();
}
AGVector2 AGWidget::innerToOuter(const AGVector2 &p) const
{
  AGVector2 m=p;

  if(mUseClientRect)
    m=mClientProj.project(m);
  return m+getRect().getV0();
}
AGRect2 AGWidget::outerToInner(const AGRect2 &p) const
{
  AGRect2 m=p;

  if(mUseClientRect)
    m=mClientProj.inverse().project(m);
  return m-getRect().getV0();
}
AGVector2 AGWidget::outerToInner(const AGVector2 &p) const
{
  AGVector2 m=p;

  if(mUseClientRect)
    m=mClientProj.inverse().project(m);
  return m-getRect().getV0();
}


void AGWidget::initEvents()
{
  if(!mEventsInited)
    eventInitEvents();
  for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
    (*i)->initEvents();
}

void AGWidget::eventInitEvents()
{

}
AGWidget *AGWidget::getFocusedWidget()
{
  AGWidget *found=0;
  for(Children::reverse_iterator i=mChildren.rbegin();i!=mChildren.rend();i++)
  {
    found=(*i)->getFocusedWidget();
    if(found)
      return found;
  }
  return canFocus()?this:0;
}

void AGWidget::setAlpha(float alpha) {
  mAlpha = alpha;
  if(alpha>0 && alpha<1)
    setCaching(true);
}

float AGWidget::getAlpha() const {
  return mAlpha;
}

