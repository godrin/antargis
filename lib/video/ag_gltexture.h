/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_gltexture.h
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


#ifndef AG_GLTEXTURE
#define AG_GLTEXTURE

#include "ag_surface.h"

class AGInternalSurface;


/**
   AGGLTexture is an internal class for managing opengl-textures.
   You shouldn't use it directly - all functionality is provided by AGTexture.
 */
class AGEXPORT AGGLTexture
  {
public:
  AGGLTexture(size_t W,size_t H,GLint format=GL_RGBA);
  AGGLTexture(size_t W,size_t H,size_t D,GLint format=GL_RGBA);

  ~AGGLTexture();

  void setSurface(AGInternalSurface *pSurface,const AGVector2 &offset=AGVector2(0,0));

  /// give gl-texture-id
  GLuint id();
  bool is3d() const;

  size_t width() const;
  size_t height() const;
  size_t depth() const;

  AGSurface getSurface() const;
  
  GLenum getTarget() const
  {
    return mTarget;
  }
  
  AGRect2 getRect() const;

private:

  void initDMABuffer();
  size_t w,h,d;

  /// is this a 3d-texture?
  bool m3d;
  bool mRectTex;
  GLenum mTarget;

  GLuint mID;


  static char *dmaBuffer;

  };

#endif

