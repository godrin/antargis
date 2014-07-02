/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_rendercontext.h
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

#ifndef __AG_RENDERCONTEXT_H
#define __AG_RENDERCONTEXT_H


#include "ag_geometry.h"
#include "ag_gltexture.h"

class AGShaderProgram;

class AGEXPORT AGRenderContext
{
 public:
  AGRenderContext();
  AGRenderContext(const AGRenderContext &c);
  virtual ~AGRenderContext();

  virtual void begin();
  virtual void end();

  void setColor(const AGVector4 &pColor);
  void setColor(const AGColor &pColor);
  void setTexture(AGGLTexture *pTexture);
  void setLighting(bool pLight);
  void setDepthWrite(bool w);
  void setDepthTest(bool t);
  void setCulling(bool c);
  void setAlpha(float v,GLuint g);

  static AGRenderContext *getCurrent();

  AGGLTexture *getTexture();
  AGVector4 *getColor();

#ifndef SWIG
  AGRenderContext &operator=(const AGRenderContext &c);
#endif

 private:
  AGVector4 *mColor;
  AGGLTexture *mTexture;
  AGShaderProgram *mProgram;

  bool mLighting;

  bool mDepthWrite;
  bool mDepthTest;
  float mAlphaThres;
  GLint mAlpha;
  bool mCulling;
};

#endif

