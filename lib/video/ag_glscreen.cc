/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_glsurface.cc
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

#include "ag_glscreen.h"
#include "SDL_image.h"
#include "ag_color.h"
#include "ag_fontengine.h"
#include "ag_sgeexport.h"
#include "ag_surfacemanager.h"
#include "ag_vdebug.h"
#include "rk_debug.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <assert.h>
#include <map>
#include <set>
#include <vector>

#include "ag_surface_internal.h"

#include <SDL.h>
#include <ag_rendercontext.h>

#include <ag_fs.h>

#include <ag_geometry.h>
#include <ag_glpainter.h>
#include <ag_profiler.h>
#include <ag_texture.h>

std::set<SDL_Surface *> glTestSurfaces;

std::set<AGGLObject *> AGGLScreen::msObjects;

void initDraw() {
  assertGL;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST); // enable depth test
  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer
  glLoadIdentity();
  glDepthMask(false);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  assertGL;
}

void initGUIView(int w, int h) {
  assertGL;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, w, 0, h);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  assertGL;
}

void myFlip() { SDL_GL_SwapBuffers(); }

////////////////////////////////////////////////////////////////////////
// AGGLObject
////////////////////////////////////////////////////////////////////////

AGGLObject::AGGLObject() { AGGLScreen::addGLObject(this); }

AGGLObject::~AGGLObject() { AGGLScreen::removeGLObject(this); }

void AGGLObject::onScreenDown() {}

void AGGLObject::onScreenUp() {}

////////////////////////////////////////////////////////////////////////
// AGGLScreen
////////////////////////////////////////////////////////////////////////

AGGLScreen::AGGLScreen(int W, int H, int VW, int VH)
    : w(VW), h(VH), rw(W), rh(H) {
  assertGL;
  if (w < rw)
    w = rw;
  if (h < rh)
    h = rh;

  mLineWidth = 2;

  // init GL
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // clear bgcolor
  glClearDepth(1.0f);                   // clear depth buffer
  glEnable(GL_DEPTH_TEST);              // enable depth test
  glDepthFunc(GL_LEQUAL);               // set type depth test
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,
         GL_NICEST); // GL_NICEST // best perspective correction
  glEnable(GL_BLEND);

  glViewport(0, 0, rw, rh);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, w, 0, h);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  assertGL;
}

AGGLScreen::~AGGLScreen() throw() {}

AGTexture AGGLScreen::screenshot(bool frontBuffer) {
  assertGL;
  throw std::runtime_error("Not implemented correctly ?");
  assert(0);
  AGTexture t(getWidth(), getHeight());

  AGGLTexture *glTexture = t.glTexture();
  AGRenderContext c;
  c.setTexture(glTexture);
  c.begin();

  if (frontBuffer)
    glReadBuffer(GL_FRONT_LEFT);
  else
    glReadBuffer(GL_BACK);

  assertGL;
#ifdef __APPLE__
  glTexParameteri(glTexture->getTarget(), GL_TEXTURE_STORAGE_HINT_APPLE,
                  GL_STORAGE_CACHED_APPLE);
  assertGL;
  glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
  assertGL;

  glTexParameteri(glTexture->getTarget(), GL_TEXTURE_STORAGE_HINT_APPLE,
                  GL_STORAGE_SHARED_APPLE);
  assertGL;
  glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
#endif
  assertGL;
  glReadBuffer(GL_FRONT_LEFT);
  glFinish();
  assertGL;
  glCopyTexSubImage2D(glTexture->getTarget(), 0, 0, 0, 0, 0, getWidth(),
                      getHeight());
  assertGL;
  // Dootherworkprocessinghere,usingadoubleortriplebuffer
  glReadBuffer(GL_BACK);

  assertGL;

  return t;
}

AGSurface AGGLScreen::screenshotSurface(bool frontBuffer) {
  assertGL;
  AGSurface s(getWidth(), getHeight());

  size_t bufSize = getWidth() * getHeight() * 4 + 16;
  glFinish();

  unsigned char *buffer = new unsigned char[bufSize];

  for (size_t i = 0; i < bufSize; i++)
    buffer[i] = 0x13;

  if (frontBuffer)
    //  glReadBuffer(GL_FRONT);
    glReadBuffer(GL_FRONT_LEFT);
  else
    glReadBuffer(GL_BACK);

  cdebug(s.surface());
  cdebug(s.surface()->surface);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  assertGL;
  glPixelStorei(GL_PACK_ROW_LENGTH, getWidth());
  assertGL;

  {
    TRACE;
    cdebug(getWidth());
    cdebug(getHeight());
    glFinish();
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE,
                 buffer); // s.surface()->surface);
    glReadBuffer(GL_BACK);
  }

  bool changed = false;
  bool everything0 = true;

  for (size_t i = 0; i < bufSize; i++) {
    if (buffer[i] != 0x13)
      changed = true;
    if (buffer[i] != 0 && buffer[i] != 0x13) {
      everything0 = false;
      // cdebug(i);
    }
  }

  cdebug("Buffer was changed:" << (changed ? "yes" : "no"));
  cdebug("Everything zero:" << (everything0 ? "yes" : "no"));

  {
    // copy
    TRACE;
    int h = getHeight() - 1;
    for (int x = 0; x < (int)getWidth(); x++)
      for (int y = 0; y < (int)getHeight(); y++) {
        unsigned char *p = buffer + ((x + (h - y) * getWidth()) * 4);
        AGColor c(p[0], p[1], p[2], p[3]);
        s.putPixel(x, y, c);
      }
  }

  assertGL;
  checkedDeleteArray(buffer);

  assertGL;

  return s;
}

void AGGLScreen::begin() {
  assertGL;
  glViewport(0, 0, rw, rh);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST); // enable depth test
  glDepthFunc(GL_LEQUAL);  // set type depth test
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,
         GL_NICEST); // GL_NICEST // best perspective correction
  glEnable(GL_BLEND);

  glViewport(0, 0, rw, rh);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, w, 0, h, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST); // enable depth test

  glDepthMask(false);
  assertGL;
}

void AGGLScreen::flip() {
  myFlip();
  getSurfaceManager()
      ->cleanup(); // FIXME: maybe delete it - or do cleanup in 2 stages
  initDraw();

  initGUIView(w, h);
}
void AGGLScreen::update(const std::list<AGRect2> &rs) { flip(); }

bool AGGLScreen::inScreen(const AGRect2 &r) const {
  return AGRect2(0, 0, w, h).contains(r);
}

void AGGLScreen::blit(const AGTexture &pSource, const AGRect2 &pRect,
                      const AGRect2 &pSrc) {
  blit(pSource, pRect, pSrc, AGColor(0xFF, 0xFF, 0xFF, 0xFF));
}

void AGGLScreen::blit(const AGTexture &pSource, const AGRect2 &pRect,
                      const AGRect2 &pSrc, const AGColor &pColor) {
  AGGLPainter::blit(pSource, pRect, pSrc, pColor);
}

void AGGLScreen::blit(
    const AGTexture &pSource,
    const std::vector<std::pair<AGRect2, AGRect2>> &pSrcDestRects,
    const AGColor &pColor) {
  AGGLPainter::blit(pSource, pSrcDestRects, pColor);
}

void AGGLScreen::tile(const AGTexture &pSource, const AGRect2 &pTarget,
                      const AGColor &pColor) {
  AGGLPainter::tile(pSource, pTarget, pColor);
}

void AGGLScreen::fillRect(const AGRect2 &pRect, const AGColor &c) {
  AGGLPainter::fillRect(pRect, c);
}

void AGGLScreen::fillRects(
    const std::vector<std::pair<AGRect2, AGVector4>> &pRects) {
  AGGLPainter::fillRects(pRects);
}

void AGGLScreen::fillPoly(const std::vector<AGVector2> &pPoly,
                          const AGColor &pColor) {
  AGGLPainter::fillPoly(pPoly, pColor);
}
void AGGLScreen::drawPoly(const std::vector<AGVector2> &pPoly,
                          const AGColor &pColor) {
  AGGLPainter::drawPoly(pPoly, pColor, mLineWidth);
}

AGRect2 AGGLScreen::getRect() const { return AGRect2(0, 0, w, h); }

void glColor(const AGColor &c) {
  assertGL;
  glColor4f(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0);
  assertGL;
}

void AGGLScreen::drawGradientAlpha(const AGRect2 &pRect, const AGColor &ul,
                                   const AGColor &ur, const AGColor &dl,
                                   const AGColor &dr) {
  AGGLPainter::drawGradientAlpha(pRect, ul, ur, dl, dr);
}

void AGGLScreen::drawBorder(const AGRect2 &rect, int W, const AGColor &c1,
                            const AGColor &c2) {
  AGGLPainter::drawBorder(rect, W, c1, c2);
}

void AGGLScreen::putPixel(int x, int y, const AGColor &pc) {
  AGGLPainter::putPixel(x, y, pc);
}

void AGGLScreen::drawGradient(const AGRect2 &rect, const AGColor &ul,
                              const AGColor &ur, const AGColor &dl,
                              const AGColor &dr) {
  drawGradientAlpha(rect, ul, ur, dl, dr);
}

void AGGLScreen::blit3dTri(const AGTexture &pSource, const AGTriangle2 &pSrc,
                           const AGTriangle3 &pDest) {
  AGGLPainter::blit3dTri(pSource, pSrc, pDest);
}

void AGGLScreen::drawLine(const AGVector2 &p0, const AGVector2 &p1,
                          const AGColor &c) {
  AGGLPainter::drawLine(p0, p1, c, mLineWidth);
}

void AGGLScreen::blitTri(const AGTexture &pSource, const AGTriangle2 &pSrc,
                         const AGTriangle2 &pDest) {
  AGGLPainter::blitTri(pSource, pSrc, pDest);
}

size_t AGGLScreen::getWidth() const { return w; }
size_t AGGLScreen::getHeight() const { return h; }
  
void AGGLScreen::setWidth(size_t width) {
  rw = w = width;
}
void AGGLScreen::setHeight(size_t height) {
  rh = h = height;
}

void AGGLScreen::clip(const AGRect2 &r) {
  // return;
#warning "insert clipping facility for opengl"
  assertGL;

  AGRect2 x = AGRect2(0, 0, w, h);
  AGRect2 m = x.intersect(r);

  GLint x0, y0, cw, ch;

  x0 = (GLint)m.x0();
  y0 = (GLint)(h - m.y1()); // -1
  cw = (GLint)m.width();
  ch = (GLint)m.height(); // +2

  assertGL;
  glScissor(x0, y0, cw, ch);
  assertGL;
  glEnable(GL_SCISSOR_TEST);
  assertGL;
}
void AGGLScreen::unclip() {
  assertGL;
  glDisable(GL_SCISSOR_TEST);
  assertGL;
}

void AGGLScreen::beginPaint() {
  assertGL;
  //  CTRACE;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, getHeight(), 0);
  glScalef(1, -1, 1);
  assertGL;
}

void AGGLScreen::endPaint() {
  assertGL;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  assertGL;
}

void AGGLScreen::setLineWidth(float w) { mLineWidth = w; }

void AGGLScreen::addGLObject(AGGLObject *pObject) { msObjects.insert(pObject); }

void AGGLScreen::removeGLObject(AGGLObject *pObject) {
  msObjects.erase(pObject);
}

void AGGLScreen::screenDown() {
  CTRACE;
  for (std::set<AGGLObject *>::iterator i = msObjects.begin();
       i != msObjects.end(); i++)
    (*i)->onScreenDown();
}
void AGGLScreen::screenUp() {
  for (std::set<AGGLObject *>::iterator i = msObjects.begin();
       i != msObjects.end(); i++)
    (*i)->onScreenUp();
}

bool opengl() {
  if (getScreenPtr()) {
    return dynamic_cast<AGGLScreen *>(getScreenPtr());
  } else {
    return false;
  }
}
