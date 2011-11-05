/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_menuitem.h
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

#ifndef AG_MENUITEM_H
#define AG_MENUITEM_H

#include "ag_text.h"
#include "ag_table.h"

class AGMenu;

class AGEXPORT AGMenuItem:public AGTable
{
 public:
  AGMenuItem(AGWidget *pParent,const AGStringUtf8 &pText);
  virtual ~AGMenuItem() throw();

  void draw(AGPainter &p);

  virtual bool eventMouseEnter();
  virtual bool eventMouseLeave();
  
  bool getSelected() const;
  void unSelect();

  AGString getName() const;

  virtual bool eventMouseClick(AGEvent *m);

  virtual void eventSelect();
  virtual void eventUnselect();

 private:
  AGStringUtf8 mText;
  bool mMouseOver;
  bool mSelected;
};

class AGEXPORT AGSubMenu:public AGMenuItem
{
 public:
  AGSubMenu(AGWidget *pParent,const AGStringUtf8 &ptext);
  virtual ~AGSubMenu() throw();

  AGMenu *getMenu();

  virtual void eventSelect();
  virtual void eventUnselect();

  virtual bool eventMouseClick(AGEvent *m);

 private:
  void addChild(AGWidget*pWidget);

  AGMenu *mSubMenu;
};

#endif
