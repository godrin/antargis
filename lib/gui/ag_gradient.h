/*
 * Copyright (c) 2008 by David Kamphausen. All rights reserved.
 *
 * ag_gradient.h
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

#ifndef AG_GRADIENT_H
#define AG_GRADIENT_H

#include "ag_color.h"

class AGGradient
  {
public:
  AGGradient();
  AGGradient(const AGColor &c0,const AGColor &c1,const AGColor &c2,const AGColor &c3);
  
  
private:
  AGColor mColors[4];
  };

#endif
