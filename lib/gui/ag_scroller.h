/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_scroller.h
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

#ifndef __AG_SCROLLER_H
#define __AG_SCROLLER_H

#include <ag_widget.h>

class AGButton;
class AGBorder;
class AGBackground;

class AGEXPORT AGScroller:public AGWidget
{
 public:
  AGScroller(AGWidget *pParent,const AGRect2 &pRect,bool pHor);
  ~AGScroller() throw();

  void draw(AGPainter &p);

  bool eventDragBy(AGEvent *pEvent);
  bool eventUpClick(AGEvent *pEvent);
  bool eventDownClick(AGEvent *pEvent);

  AGSignal sigValueChanged;

  void setValue(float pValue);
  void setScrollerSize(float pSize);
  void setInterval(float pMin,float pMax);
  void setStepping(float pStep);

  float getValue() const;
  float getMin() const;
  float getMax() const;

 private:

  /// width of scroller if hor (otherwise height)
  float getScrollerWidth();
  /// height of scroller if hor (otherwise width)
  float getScrollerHeight();
  float getMovingWidth();

  void updateScroller();


  float mValue;
  float mValueWidth;
  float mMin,mMax;
  float mStepping;

  float mVirtualValue;

  float mBorderWidth;

  bool mHorizontal;
  AGButton *mB1,*mB2;
  AGButton *mScroller;
  AGBorder *mBorder;
  AGBackground *mBackground;
};


#endif
