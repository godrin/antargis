/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_colorbutton.h
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

#ifndef AG_COLORBUTTON_H
#define AG_COLORBUTTON_H

#include "ag_widget.h"
#include "ag_color.h"
#include <vector>

class AGEXPORT AGColorButton:public AGWidget
{
 public:
  AGColorButton(AGWidget *pParent,const AGRect2 &r,int x,int y);
  virtual ~AGColorButton() throw();

  virtual void draw(AGPainter &p);

  AGColor getColor() const;
  void setColor(const AGColor &c);
  void setColor(int x,int y,const AGColor &c);

  virtual bool eventMouseClick(AGEvent *m);

 private:
  int gridx,gridy;
  std::vector<AGColor> mColors;

  AGColor mColor;
};

AGEXPORT AGColorButton &toAGColorButton(AGWidget &pWidget);

#endif
