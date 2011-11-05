/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_edit.h
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

#ifndef __AG_EDIT_H
#define __AG_EDIT_H

#include "ag_widget.h"
#include "ag_font.h"
#include "ag_surface.h"
#include "ag_background.h"
#include "rk_utf8.h"
#include "ag_string_utf8.h"

enum AGAlign {EDIT_LEFT,EDIT_RIGHT,EDIT_JUSTIFY,EDIT_CENTER};
enum AGVAlign {EDIT_TOP,EDIT_BOTTOM,EDIT_VCENTER};

class AGEXPORT AGEditLine
{
 public:
  AGEditLine(const AGStringUtf8 &pText,AGFont pFont,bool pHardEnd);
  virtual ~AGEditLine();

  void setAlign(AGAlign pAlign);
  void setVAlign(AGVAlign pVAlign);

  virtual void draw(AGPainter &p,const AGVector2 &pPoint,const AGRect2 &pClip);
  virtual void drawCursor(AGPainter &p,int cx,const AGVector2 &pPoint,const AGRect2 &pClip,const AGColor &c);

  int height() const;
  int width() const;

  void insert(const AGStringUtf8 &c,int cx,bool pInsert);
  void doDelete(int cx);
  AGEditLine split(int cx);

  int length() const;

  void append(const AGStringUtf8 &s);
  void prepend(const AGStringUtf8 &s);
  AGStringUtf8 getText() const;
  AGFont getFont() const;
  void setText(const AGStringUtf8 &s);

  bool hardEnd() const;
  void setHardEnd(bool pHardEnd);
  std::pair<AGStringUtf8,bool> checkUnwrap(int pW,const AGStringUtf8 &s);

  std::pair<AGStringUtf8,bool> checkWrap(int pW);

  void setFont(const AGFont &pFont);

 private:
  AGStringUtf8 mText;
  AGFont mFont;
  bool mHardEnd;
  AGAlign mAlign;
  AGVAlign mVAlign;
};

class AGEXPORT AGEdit:public AGWidget
{
 public:
  AGEdit(AGWidget *pParent,const AGRect2 &pRect);
  virtual ~AGEdit()  throw();

  virtual void draw(AGPainter &p);
  virtual void drawBackground(AGPainter &p);

  virtual bool eventKeyDown(AGEvent *m);
  virtual bool eventKeyUp(AGEvent *m);

  void setMulti(bool mMultiLine);

  bool insert(const AGStringUtf8 &c);
  void doDelete(int p);

  void setText(const AGStringUtf8 &pText);
  void setFont(const AGFont &pFont);

  void clear() throw();

  void setMutable(bool pMutable);

  void setAlign(AGAlign pAlign);
  void setVAlign(AGVAlign pVAlign);

  void setBackground(bool pDrawBackground);
  void setBackground(const AGBackground &bg);

  void setTheme(const AGString &s);

  bool canFocus() const;

  AGStringUtf8 getText() const;

  void setMaxLength(int i);

  virtual bool eventGotFocus();
  virtual bool eventLostFocus();

  virtual void prepareDraw();

  void setInsert(bool pInsert);

  void setCursor(int p);

  AGSignal sigModified;

 private:

  void getActLine();
  void insertLine(AGEditLine l);
  void mergeLine(int p);
  void checkWrap();
  bool processKey(SDLKey k,Uint16 unicode);

  std::list<AGEditLine> mLines;
  AGEditLine *actLine;

  int mCx,mCy; // cursor position
  int mViewCy;

  Uint32 mCursorTime;
  Uint32 mCursorLast;
  bool mShowCursor;
  bool mLShift,mRShift;
  bool mLCtrl,mRCtrl;
  bool mLAlt,mRAlt;

  bool mInserting;

  bool mMultiLine;

  bool mWrapLines;

  AGBackground mBackground;

  AGFont mFont;

  AGAlign mAlign;
  AGVAlign mVAlign;

  bool mMutable;
  bool mDrawBackground;

  AGString mTheme;

  int mMaxLength;
};

#endif
