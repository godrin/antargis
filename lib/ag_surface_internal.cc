/*
 * Copyright (c) 2009 by David Kamphausen. All rights reserved.
 *
 * ag_surface_internal.cc
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

#include "ag_surface_internal.h"
#include "ag_surfacemanager.h"

AGInternalSurface::AGInternalSurface()
{
  sdlTexture=0;
  glTexture=0;
  surface=0;
  getSurfaceManager()->registerMe(this);
  version=1;
}

AGInternalSurface::~AGInternalSurface()
{
  getSurfaceManager()->deregisterMe(this);
}
