/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_menu.h
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

#ifndef AG_MENU_H
#define AG_MENU_H

#include "ag_table.h"
#include "ag_texture.h"

class AGEXPORT AGMenu:public AGTable
{
 public:
  AGMenu(AGWidget *pParent,AGVector2 pWishPos,const AGStringUtf8 &pName);
  virtual ~AGMenu() throw();

  void show(AGVector2 pWishPos);

  void addItem(const AGStringUtf8 &pString);
  AGMenu &addMenu(const AGStringUtf8 &pString);

  void draw(AGPainter &p);

  void eventItemSelected(const AGString &pString);
  void eventItemClicked(const AGString &pString);

  AGSignal sigSelected;

 private:
  AGTexture mBg;
  AGString mName;
  AGVector2 mWishPos;

  float mW,mH;
};

#endif
