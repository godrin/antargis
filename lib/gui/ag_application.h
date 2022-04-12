/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_application.h
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

#ifndef AG_APPLICATION_H
#define AG_APPLICATION_H

#include <ag_messageobject.h>
#include <ag_texture.h>
#include <ag_tooltip.h>
#include <ag_widget.h>

/**
   \defgroup application Application
*/

/**
   \brief Base class for application - holds the main loop
   \ingroup application

  Use AGApplication as base class your application's class.
  In this library it's assumed that you have different parts of your program,
  that are really different. Each of these parts has its own AGApplication. The
  actual control of these applications is done through a main-loop, that queries
  new events from libSDL and gives them to the correct widgets. This main-loop
  can by started by run(). After this your have some possibilities to access
  control. FOr instance there are the different event* functions, which you can
  override in an AGApplication's subclass.

  If you want to quit this application call tryQuit().

  Each AGApplication has a single main-widget, which is drawn in every frame.
  Set it by calling setMainWidget().

  The typical usage looks like this:
  <pre>
  class MyApp:public AGApplication
  {
    public:
    bool eventFrame(float t)
    {
      std::cout<<"In this frame "<<t<<" seconds have passed."<<std::endl;
    }
  };

  int main()
  {
    // here you have to init the video mode
    AGMain main;
    main.initVideo(800,600,32,false,false);

    // the actual example
    MyApp app;
    app.run();
    return 0;
  }
  </pre>
*/
class AGEXPORT AGApplication : public AGMessageObject {
public:
  AGApplication();
  virtual ~AGApplication() throw();

  bool run();

  /// called in each frame when idling
  virtual bool eventIdle();

  virtual bool eventPrepareFrame(float pTime);

  /// called between event-handling and drawing
  virtual bool eventFrame(float pTime);
  /// called after drawing - so before event handling
  virtual bool eventFrameEnd(float pTime);

  virtual bool eventQuit(AGEvent *m);
  virtual bool eventKeyDown(AGEvent *m2);
  virtual bool eventMouseMotion(AGEvent *m);

  virtual void eventChangedRes();

  void setMainWidget(AGWidget *w);
  AGWidget *getMainWidget();

  virtual void draw();
  virtual void prepareDraw();

  void redraw();

  virtual void tryQuit();
  bool isRunning() const;

  long getTicks() const;
  void delay(int ms);

  void setTooltip(AGTooltip *pTooltip);   // transfers ownage !
  void resetTooltip(AGTooltip *pTooltip); // try to reset this one

  void setOverlay(AGWidget *pOverlay);
  AGWidget *getOverlay();

  void setCursor(const AGTexture &pTexture);
  void setNormalCursor();
  bool hardwareCursor() const;

  virtual SDL_Event getNewEvent();

  void setKeyRepeat(bool enable);

  void setDemoTime(float t);

  bool doEvent(const SDL_Event &e);

  AGSignal sigFrameFinished;

private:
  void clearOldMousePosition();
  void drawCursor();

  void flushEventQueue();

  bool mRunning;
  bool mIdleCalls;
  AGWidget *mainWidget;
  AGTooltip *mTooltip;

  AGWidget *mOverlay;

  AGTexture *mCursor;
  AGRect2 mCursorOld;

  std::list<AGWidget *> delCue;

  SDL_Event mEvent;

  float mDemoTime;
};

/**
   getApplication returns the current active application.
   This gets set on each iteration of AGApplication::run.
   So it will be correct after finishing a frame, when you are
   using nested AGApplications.
*/
// AGEXPORT AGApplication *getApplication(); // returns current active
// application

AGEXPORT void disableKeyrepeat();

#endif
