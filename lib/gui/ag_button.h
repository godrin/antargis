/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_button.h
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

#ifndef AG_BUTTON_H
#define AG_BUTTON_H

#include "ag_widget.h"
#include "ag_surface.h"
#include "ag_background.h"
#include "ag_border.h"

#include <iostream>
#include <map>

class AGEdit;
class AGImage;

/**
   \brief implements push-buttons
   \ingroup widgets

   AGButton represents a button, which can hold a text-caption or an image-caption, but not both.
   Further it can have following states:
*/

class AGEXPORT AGButton:public AGWidget
{
 public:
  /// the different drawing states
   enum State {
     NORMAL,         //!< the normal state of a button
     LIGHTED,        //!< the button is lighted or hovered
     PRESSED,        //!< the button is pressed (typically hovered)
     CHECKED,        //!< the button is checked (and not pressed)
     CHECKEDLIGHTED, //!< a hovered and checked button
     CHECKEDPRESSED, //!< hovered, checked and pressed
     DISABLED        //!< the button disabled (greyed)
   };

  AGButton(AGWidget *pParent,const AGRect2 &r,const AGStringUtf8&pText="",int id=-1);
  virtual ~AGButton() throw();

  void setSurface(AGSurface pSurface,bool pChangeSize=false);
  void setTexture(const AGTexture &pTexture);

  virtual void draw(AGPainter &p);

  virtual bool eventMouseEnter();
  virtual bool eventMouseLeave();

  virtual bool eventMouseButtonDown(AGEvent *m);
  virtual bool eventMouseButtonUp(AGEvent *m);

  virtual void setWidth(float w);
  virtual void setHeight(float w);
  void setRect(const AGRect2 &r);

  virtual AGStringUtf8 getCaption() const;

  void setEnabled(bool pEnable);

  void setTheme(const AGString &pTheme);

  void setCaption(const AGStringUtf8 &pCaption);

  /// sets the current state and ensures a redraw
  virtual void setState(const State &pState);
  State getState() const;

  virtual void setChecked(bool pChecked);
  bool isChecked() const;

  bool canFocus() const;

  virtual void useTextures();

  bool isOpaque() const;

 private:
   
  void updateClientRects();
  AGStringUtf8 mText;
  int mID;
  AGString mTheme;
  State mState;
  int borderWidth;
  AGSurface mSurface;
  AGSurface mGrayedSurface;
  bool mHasSurface;
  AGEdit *mTextW;
  AGImage *mImageW;

  bool lower;
  bool mChecked;

  std::map<State,AGBackground> mBG;
  std::map<State,AGBorder> mBorder;
  bool mEnabled;
};

AGEXPORT AGButton &toAGButton(AGWidget &pWidget);

#endif
