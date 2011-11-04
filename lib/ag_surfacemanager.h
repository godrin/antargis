/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_surfacemanager.h
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

#ifndef AG_SURFACEMANAGER_H
#define AG_SURFACEMANAGER_H

#include <rk_base.h>
#include <set>
#include <cstdlib>

class AGRenderContext;
class AGSurface;
class AGTexture;
class AGInternalSurface;
class AGGLTexture;

class AGEXPORT AGSurfaceManager
{
 public:
  ~AGSurfaceManager();

  // AGSurfaces are not owned - but SDL_Surfaces - so beware!!
  void registerMe(AGSurface *pSurface);
  void deregisterMe(AGSurface *pSurface);

  void registerMe(AGTexture *pTexture);
  void deregisterMe(AGTexture *pTexture);

  void registerMe(AGInternalSurface *p);
  void deregisterMe(AGInternalSurface *p);

  void registerMe(AGRenderContext *p);
  void deregisterMe(AGRenderContext *p);

  void registerMe(AGGLTexture *p);
  void deregisterMe(AGGLTexture *p);

  void cleanup(bool force=false, bool nomem=false);
  void clear();

  size_t getUsedTexMem() const;

 private:
  AGSurfaceManager();

  std::set<AGSurface*> mSurfaces;
  std::set<AGInternalSurface*> mSDLSurfaces;
  std::set<AGTexture*> mTextures;
  std::set<AGGLTexture*> mGLTextures;
  std::set<AGRenderContext*> mRContext;

  friend AGSurfaceManager *getSurfaceManager();
};
AGEXPORT AGSurfaceManager *getSurfaceManager();

#endif

