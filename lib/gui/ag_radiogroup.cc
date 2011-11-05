/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_radiogroup.cc
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


#include "ag_radio.h"
#include "ag_radiogroup.h"
#include "ag_local.h"
#include "ag_theme.h"
#include "ag_image.h"
#include "ag_layoutfactory.h"
#include <typeinfo>


AGRadioGroup::AGRadioGroup(AGWidget *p,const AGRect2 &r):
  AGWidget(p,r),
  sigChanged(this,"sigChanged")
  {
  }
AGRadioGroup::~AGRadioGroup() throw()
  {
    std::set<AGRadio*>::iterator i=mChildren.begin();

    for(;i!=mChildren.end();i++)
      (*i)->setGroup(0);
  }

void AGRadioGroup::eventChange(const AGString &p)
  {
    std::set<AGRadio*>::iterator i=mChildren.begin();

    for(;i!=mChildren.end();i++)
      {
        if((*i)->getName()!=p)
          (*i)->deselect();
      }

  }

void AGRadioGroup::add(AGRadio *r)
  {
    mChildren.insert(r);
  }
void AGRadioGroup::erase(AGRadio *r)
  {
    mChildren.erase(r);
  }





