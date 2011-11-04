/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_fbo.cc
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


#include "GLee.h"
#include "ag_fbo.h"
#include "ag_surface.h"
#include "ag_vdebug.h"
#include "ag_config.h"

#include <stdexcept>

#define USE_FBO

AGFBO::AGFBO(AGGLTexture *pTexture, bool withDepth) :
mTexture(pTexture),
mWithDepth(withDepth) {
  mTextureID = mTexture->id();
  w = mTexture->width();
  h = mTexture->height();
  fb = depth_rb = 0;

  std::cerr << "WARNING: FBOs can make problems on some cards and drivers. Disable them in ~/.Antargis/config.xml !" << std::endl;

  init();
}


// init for depth-only writing

AGFBO::AGFBO(GLuint pTexture, size_t pW, size_t pH) :
mTexture(0), mTextureID(pTexture),
mWithDepth(false) {
  fb = depth_rb = 0;
  w = pW;
  h = pH;
  std::cerr << "WARNING: FBOs can make problems on some cards and drivers. Disable them in ~/.Antargis/config.xml !" << std::endl;


#ifdef USE_FBO

  glGenFramebuffersEXT(1, &fb);
  if (mWithDepth)
    glGenFramebuffersEXT(1, &depth_rb); // FIXME: this should read glGenRenderbuffersEXT

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


  glBindTexture(GL_TEXTURE_2D, mTextureID);

  assertGL;
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mTextureID, 0);
  assertGL;

  {
    // add a renderbuffer as color attachment

    glGenRenderbuffersEXT(1, &depth_rb);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, w, h);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, depth_rb);


  }

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

#endif
}

void AGFBO::init() {
#ifdef USE_FBO
  glGenFramebuffersEXT(1, &fb);
  if (mWithDepth)
    glGenFramebuffersEXT(1, &depth_rb);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


  glBindTexture(GL_TEXTURE_2D, mTextureID);

  assertGL;
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTextureID, 0);
  assertGL;

  if (mWithDepth) {
    throw std::runtime_error("AGFBO:not supported yet!");
    // initialize depth renderbuffer
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, w, h); //pTexture->width(), pTexture->height());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
  }
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  assertGL;
#endif
}

AGFBO::~AGFBO() {
#ifdef USE_FBO
  CTRACE;
  if (fb)
    glDeleteFramebuffersEXT(1, &fb);
  if (depth_rb)
    glDeleteRenderbuffersEXT(1, &depth_rb);
#endif
}

void AGFBO::beginDraw() {
#ifdef USE_FBO
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
  assertGL;

#endif
}

void AGFBO::endDraw() {
#ifdef USE_FBO
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  assertGL;

#endif
}




#warning "add global var for this!"

bool canFBO() {
  static int meCanFBO = -1;
  if (meCanFBO < 0) {
#ifndef USE_FBO
    meCanFBO = 0; // FIXME: FBO painting does not work yet
#else

    if (getConfig()->get("useFBO", "false", "<!--enable Frame-Buffer-Objects - can make problems on some drivers. options:true,false -->") == "true")
      meCanFBO = GLEE_EXT_framebuffer_object ? 1 : 0;
    else
      meCanFBO = 0;
#endif
  }
  return meCanFBO > 0;
}
