/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_window.h
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

#ifndef AG_WINDOW_H
#define AG_WINDOW_H

#include "ag_table.h"

class AGEXPORT AGWindow:public AGTable
{
 public:
  AGWindow(AGWidget *pWidget,const AGRect2 &pRect,const AGStringUtf8 &pTitle="",const AGString &pTheme="");
  virtual ~AGWindow()  throw();

  AGWidget *getClient();

  virtual bool eventMouseButtonDown(AGEvent *m);
  bool eventDragBy(AGEvent *event,const AGVector2 &pDiff);

  void close();

  // slot
  virtual bool tryClose(AGEvent *m);

  AGRect2 getClientRect() const;

  AGSignal sigClose;

 private:

  AGWidget *getTitleBar(int w,int h);

  AGStringUtf8 mTitle;
  AGWidget *mClient;
};

//AGWindow &toAGWindow(AGWidget &w);

#endif
