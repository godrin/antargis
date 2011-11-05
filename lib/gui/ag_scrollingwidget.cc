/*
 * Copyright (c) 2008 by David Kamphausen. All rights reserved.
 *
 * ag_scrollingwidget.cc
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

#include "ag_scrollingwidget.h"
#include "ag_layoutfactory.h"
#include "ag_painter.h"

AGScrollingWidget::AGScrollingWidget(AGWidget *pParent, const AGRect2& pRect):
  AGWidget(pParent,pRect),
  mDragging(false)
    {
      AGRect2 r=getRect().origin();
      setClient(r,AGProjection2D(r,r));
    }

void AGScrollingWidget::setClientRect(const AGRect2 &pRect)
  {
    AGRect2 r=getRect().origin();
    setClient(pRect,AGProjection2D(r,r));
  }

void AGScrollingWidget::setVector(const AGVector2 &pVector)
  {
    AGProjection2D p=getClientProjection();
    AGRect2 from=getRect().origin();
    AGRect2 to=p.project(from);
    AGVector2 v=pVector;
    v=-getClientWorld().shrinkToTopLeft(from.width(),from.height()).clip(-v);
    setClient(getClientWorld(),AGProjection2D(from,to.origin()+v));
    
  }

AGVector2 AGScrollingWidget::getVector() const
{
  AGProjection2D p=getClientProjection();
  return p.project(AGVector2(0,0));
}


bool AGScrollingWidget::eventMouseButtonDown(AGEvent *pEvent)
  {
    bool result=AGWidget::eventMouseButtonDown(pEvent);
    if(hovered())
      {
        mDragging=true;
        cdebug("dragging!");
      }
    return result;
  }
bool AGScrollingWidget::eventMouseButtonUp(AGEvent *pEvent)
  {
    if(mDragging)
      {
        cdebug("No longer dragging!");

        mDragging=false;
      }
    return AGWidget::eventMouseButtonUp(pEvent);
  }
bool AGScrollingWidget::eventDragBy(AGEvent *pEvent,const AGVector2 &pVector)
  {
    if(mDragging)
      {
        setVector(getVector()+pVector);
      }
    return AGWidget::eventDragBy(pEvent,pVector);
  }


