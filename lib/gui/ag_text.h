/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_text.h
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

#ifndef AG_TEXT_H
#define AG_TEXT_H

#include "ag_widget.h"
#include "ag_font.h"

class AGEXPORT AGText:public AGWidget
{
 public:
  AGText(AGWidget *pParent,const AGRect2 &pRect,const AGStringUtf8 &pText,const AGFont &pFont);
#ifndef SWIG
  AGText(AGWidget *pParent,const AGVector2 &pPoint,const AGStringUtf8 &pText,const AGFont &pFont);
#endif
  virtual ~AGText() throw();
  
  void setDeriveRect();
  virtual void draw(AGPainter &p);

  void setText(const AGStringUtf8 &pText);
  AGStringUtf8 getText() const;

  void setFont(const AGFont &f);

 private:
  AGStringUtf8 mText;
  AGFont mFont;
  bool mFixedSize;
};

//AGText &toAGText(AGWidget &w);

#endif
