/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_widget.h
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

#ifndef AG_WIDGET_H
#define AG_WIDGET_H

#include "rk_base.h"
#include "ag_geometry.h"
#include "ag_messageobject.h"
#include "ag_projection.h"

#include <list>

#include "ag_painter.h"

class AGTooltip;
class AGLayout;
class AGClipping;
class AGApplication;

/**
    \defgroup widgets Widgets
    \brief contains all the widgets

    Everything that has a display on screen is called a widget. There are plenty of different widget-types
    and you are free to define new ones.

    An AGApplication object has one single main-widget, that you can define.
    All other widgets are children (or grandchildren ...) of this main-widget.

    A simple example how to use nested widgets is:
    <pre>
    AGWidget *parent,*w;
    AGApplication app;

    parent=new AGWidget(0,AGRect2(0,0,640,480));
    w=new AGWidget(parent,AGRect2(0,0,100,100));
    parent->addChild(w);
    app.setMainWidget(parent);
    </pre>

    Nearly all of the widget-types are created this way. Adding a widget to another will automate
    the drawing and event-processing.

 */



/**
   \brief base class for all widgets

   \ingroup widgets

   AGWidget is the base class for all widgets. It manages the hierarchy and memory of the widgets.
   You should never deletee an AGWidget for yourself if you don't know exactly what you're doing.
   The widgets get deleted by their parents
 */

class AGEXPORT AGWidget:public AGMessageObject
{
public:
  typedef std::list<AGWidget*> Children;

  AGWidget(AGWidget *pParent,const AGRect2 &r);
  virtual ~AGWidget() throw();

  AGApplication *getApp();
  void setApp(AGApplication *pApp);

  virtual void initHandlers();

  virtual void draw(AGPainter &p);
  virtual void drawAfter(AGPainter &p);
  virtual void drawAll(AGPainter &p);
  virtual void drawChild(AGPainter &p,AGWidget *pWidget);

  AGRect2 getRect() const;
  virtual AGRect2 getClientRect() const;
  virtual void setRect(const AGRect2 &pRect);

  virtual void setParent(AGWidget *pParent);
  AGWidget *getParent();
  bool isParent(AGWidget *pParent);

  virtual bool eventShow();
  virtual bool eventHide();

  virtual bool eventMouseMotion(AGEvent *m);

  virtual bool eventMouseEnter();
  virtual bool eventMouseLeave();
  virtual bool eventMouseClick(AGEvent *m);

  virtual bool eventMouseButtonDown(AGEvent *m);
  virtual bool eventMouseButtonUp(AGEvent *m);
  void setButtonDown(bool value,const AGVector2 &startVector);

  virtual bool eventGotFocus();
  virtual bool eventLostFocus();

  virtual void eventTick(float pTime);
  void sigTick(float pTime);

  virtual bool canFocus() const;

  bool processEvent(AGEvent *event);

  bool doClick(AGEvent *e);

  AGSignal sigMouseEnter;
  AGSignal sigMouseLeave;
  AGSignal sigClick;
  AGSignal sigDragBy;

  virtual float minWidth() const;
  virtual float minHeight() const;

  float width() const;
  float height() const;
  float top() const;
  float left() const;
  float bottom() const;
  float right() const;

  bool visible() const;
  void setVisible(bool v);
  bool isOpaque() const;

  virtual void setWidth(float w);
  virtual void setHeight(float w);
  virtual void setTop(float y);
  virtual void setLeft(float x);

  bool fixedWidth() const;
  bool fixedHeight() const;

  void show();
  void hide();
  void close();

public:

  virtual void addChild(AGWidget *w);
  virtual void addChildBack(AGWidget *w);

  virtual void removeChild(AGWidget *w);

  // Functions for caching appearance
  virtual bool redraw() const;
  virtual void prepareDraw();

  void prepareDrawAll();

  void setCaching(bool pEnable);
  bool checkRedraw() const;
  void setDrawn();
  void queryRedraw();
  AGRect2 getChangeRect();

  // when caching, "normal" textures are not used, so they get discarded
  // to prevent this, use this function to "use" them in each frame
  virtual void useTextures(); // when caching
  void useTexturesRecursive(); // when caching




  // geometry calculation
  AGRect2 innerToOuter(const AGRect2 &pRect) const;
  AGVector2 innerToOuter(const AGVector2 &pRect) const;
  AGRect2 outerToInner(const AGRect2 &pRect) const;
  AGVector2 outerToInner(const AGVector2 &pRect) const;


  // clear children
  void clear() throw();

  AGRect2 getScreenRect() const;
  AGRect2 toScreen(const AGRect2&p) const;
  AGVector2 toScreen(const AGVector2&p) const;
  AGRect2 fromScreen(const AGRect2 &p) const;
  AGVector2 fromScreen(const AGVector2 &p) const;

  AGProjection2D innerToOuter() const;
  AGProjection2D outerToInner() const;

  AGVector2 getScreenPosition() const;


  // focus

  /** should only be called by a child */
  void gainFocus(AGWidget *pWidget=0);
  void gainCompleteFocus(AGWidget *pWidget=0);


  virtual bool eventDragBy(AGEvent *event,const AGVector2 &pDiff);

  bool getFocus() const;
  bool hasFocus(const AGWidget *pWidget=0);
  AGWidget *getFocusedWidget ();

  AGLayout *getLayout();


  const AGString &getName() const;
  void setName(const AGString &pName);
  AGWidget *getChild(const AGString &pName);

  void setModal(bool pModal);

  void erase(AGWidget *w);

  void eventChildrenDeleted(AGWidget *pWidget);

  void setTooltip(const AGStringUtf8 &pTooltip);

  void acquireClipping(AGClipping &p);

  std::list<AGRect2> aquireChanges();
  void pushChangeRect(const AGRect2 &pRect);
  void clearChangeRects();

  std::list<AGWidget*> getChildren();

  bool hovered() const;

  void setClient(const AGRect2 &pWorld,const AGProjection2D &pProj);
  AGProjection2D getClientProjection() const;
  AGRect2 getClientWorld() const;


  virtual bool eventMouseButtonDownClipped(AGEvent *pEvent,const AGVector2 &pPosition);
  virtual bool eventMouseButtonUpClipped(AGEvent *pEvent,const AGVector2 &pPosition);
  virtual bool eventMouseMotionClipped(AGEvent *pEvent,const AGVector2 &pPosition);

  void addChildRef(AGWidget *pWidget);

  void initEvents();

  virtual void eventInitEvents();


protected:
  virtual bool letChildProcess(AGWidget *pChild,AGEvent *event);

private:

  AGRect2 getChildRect(const AGRect2 &pRect) const;
  bool containsPoint(AGWidget *pWidget,const AGVector2 &pVector) const;


  AGApplication *mApp;

  std::list<AGRect2> mMyChanges;

  void regChange();

  void drawCache();

  void delObjects();

  void gainFocusDown(AGWidget *pWidget);

  void checkFocus();

  static bool valid(AGWidget *pWidget);

  std::list<AGWidget*> mToClear;

  AGRect2 mRect,mClientWorld;
  AGProjection2D mClientProj;
  bool mUseClientRect;

  AGWidget *mParent;
  bool mChildrenEventFirst;
  bool mChildrenDrawFirst;
  bool mMouseIn;
  bool mButtonDown;
  bool mFixedWidth,mFixedHeight;
  bool mVisible;
  bool mCaching;
  AGTexture *mCache;
  bool mCacheTouched;

  bool mHasFocus;
  AGWidget *mFocus;

  AGVector2 mOldMousePos;

  AGString mName;
  bool mModal;

  AGRect2 mChangeRect;

  AGStringUtf8 mTooltip;
  AGTooltip *mTooltipWidget;

  std::set<AGWidget*> mRefChildren;

  bool mEventsInited;

protected:
  std::list<AGWidget*> mChildren;

private:
  static std::set<AGWidget*> allWidgets;

};

AGEXPORT void setNewClippingTechnique(bool f);
AGEXPORT bool getNewClippingTechnique();
AGEXPORT void printStacktrace();


#endif
