/*
 * Copyright (c) 2009 by David Kamphausen. All rights reserved.
 *
 * ag_surface_internal.h
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

#ifndef AG_SURFACE_INTERNAL
#define AG_SURFACE_INTERNAL

#include "rk_base.h"

#include "ag_gltexture.h"

class AGGLTexture;

/**
   AGInternalSurface links plain sdl-surfaces (in main-memory) and textures (both gl-textures and
   sdl-screenlike-surfaces)
*/
struct AGEXPORT AGInternalSurface
{
  AGInternalSurface  *sdlTexture;
  AGGLTexture *glTexture;
  SDL_Surface *surface;
  size_t version;

  AGInternalSurface();
  ~AGInternalSurface();
};


#endif
