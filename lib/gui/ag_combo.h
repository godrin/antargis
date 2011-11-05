/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_combo.h
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

#ifndef AG_COMBO_H
#define AG_COMBO_H

#include <ag_widget.h>

class AGEdit;
class AGButton;
class AGListBox;

class AGEXPORT AGComboBox:public AGWidget
{
 public:
  AGComboBox(AGWidget *pParent,const AGRect2 &pRect);
  virtual ~AGComboBox() throw();

  void insertItem(const AGString &pID,const AGStringUtf8 &pContent);

  bool eventButtonClicked(AGEvent *pEvent);
  bool eventSelected(AGEvent *pEvent);

  AGSignal sigSelect;
  
  AGString getSelected() const;
  void setSelected(const AGString &pID);

  void clear() throw();
  
  virtual void setWidth(float w);
  virtual void setHeight(float w);
  void setRect(const AGRect2 &r);

 private:
  void update();
  void updateClientRects();

  AGEdit *mEdit;
  AGButton *mButton;
  AGListBox *mListBox;

  AGString mID;

  std::list<std::pair<AGString,AGStringUtf8> > ops;
};

#endif
