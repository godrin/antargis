/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_local.cc
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

#include "ag_local.h"
#include "rk_debug.h"


AGLocalizer *gLocalizer=0;

AGLocalizer::~AGLocalizer() throw()
  {
    if(gLocalizer==this)
      gLocalizer=0;
  }

AGStringUtf8 AGLocalizer::find(const AGString &p)
  {
    cdebug("WARNING: Default Localizer used!");
    return AGStringUtf8(p);
  }

void setLocalizer(AGLocalizer *p)
  {
    gLocalizer=p;
  }

AGLocalizer *getLocalizer()
  {
    return gLocalizer;
  }


AGStringUtf8 translate(const AGString &s)
  {
    AGLocalizer *l=getLocalizer();

    if(l)
      return l->find(s);
    else
      cdebug("No Localizer found!");
    return AGStringUtf8(s);
  }
