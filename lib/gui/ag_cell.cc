/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_cell.cc
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

#include "ag_cell.h"

AGCell::AGCell(AGWidget *pParent,const AGRect2 &pRect)
:AGWidget(pParent,pRect)
    {

    }
void AGCell::setWidth(float w)
  {
    AGWidget::setWidth(w);
    for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
      (*i)->setWidth(w);
  }
void AGCell::setHeight(float w)
  {
    AGWidget::setHeight(w);
    for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
      (*i)->setHeight(w);
  }
/*
void AGCell::setTop(float w)
  {
    AGWidget(setTop(w));
    for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
      (*i)->setTop(w);
  }
void AGCell::setLeft(float w)
  {
    for(Children::iterator i=mChildren.begin();i!=mChildren.end();i++)
      (*i)->setLeft(w);
  }
*/
