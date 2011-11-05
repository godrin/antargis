/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_painter.cc
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

#include <rk_debug.h>
#include <ag_draw.h>
#include <ag_fontengine.h>
#include <ag_glscreen.h>
#include <ag_painter.h>
#include <ag_profiler.h>
#include <ag_sdlsurface.h>
#include <ag_texture.h>
#include <cmath>
#include <ag_geometry.h>
#include "ag_video.h"

#include <stdexcept>

//#define SPEED_TEST

/////////////////////////////////////////////////////////////////////////////////
// AGProjection
/////////////////////////////////////////////////////////////////////////////////

AGPaintProjection::AGPaintProjection(const AGRect2 &pClip) : clip(pClip) {
  a.set(0, 0, 1);
  a.set(0, 1, 0);
  a.set(0, 2, 0);
  a.set(1, 0, 0);
  a.set(1, 1, 1);
  a.set(1, 2, 0);
  a.set(2, 0, 0);
  a.set(2, 1, 0);
  a.set(2, 2, 0);
}

AGPaintProjection::AGPaintProjection(const AGClipping &pClip) : advancedClipping(pClip) {
  a.set(0, 0, 1);
  a.set(0, 1, 0);
  a.set(0, 2, 0);
  a.set(1, 0, 0);
  a.set(1, 1, 1);
  a.set(1, 2, 0);
  a.set(2, 0, 0);
  a.set(2, 1, 0);
  a.set(2, 2, 0);
}

AGVector2 AGPaintProjection::project(const AGVector2 &p) const {
  AGVector2 r = (a * AGVector3(p[0], p[1], 1)).dim2();

  return r;
}

bool AGPaintProjection::pointOk(const AGVector2 &p) const {
  return clip.contains(p);
}

AGRect2 AGPaintProjection::project(const AGRect2 &p) const {
  AGRect2 r((a * AGVector3(p[0], 1)).dim2(),
          (a * AGVector3(p[1], 1)).dim2());
  return r;
}

AGRect2 AGPaintProjection::clipRect(AGRect2 target) const {
  return clip.intersect(target);
}

std::pair<AGRect2, AGRect2> AGPaintProjection::clipRect(AGRect2 target, AGRect2 src) const {
  AGRect2 i = clip.intersect(target);
  if (i.width() <= 0 || i.height() <= 0)
    return std::make_pair(AGRect2(0, 0, 0, 0), AGRect2(0, 0, 0, 0));

  assert(target.x0() < clip.x1());


  // clip left
  if (target.x0() < clip.x0()) {
    float a = (clip.x0() - target.x0()) / target.w();
    float nw = target.x1() - clip.x0();
    target.setLeft(clip.x0());
    target.setWidth(nw);
    nw = src.w()*(1.0f - a);
    src.setLeft(src.x0() + src.w() * a);
    src.setWidth(nw);
  }

  // clip top
  if (target.y0() < clip.y0()) {
    float a = (clip.y0() - target.y0()) / target.h();
    float nh = target.y1() - clip.y0();
    target.setTop(clip.y0());
    target.setHeight(nh);
    nh = src.h()*(1.0f - a);
    src.setTop(src.y0() + src.h() * a);
    src.setHeight(nh);
  }

  // clip right
  if (target.x1() > clip.x1()) {
    float a = (target.x1() - clip.x1()) / target.w();
    target.setRight(clip.x1());
    src.setRight(src.x1() - src.w() * a);
  }

  // clip bottom
  if (target.y1() > clip.y1()) {
    float a = (target.y1() - clip.y1()) / target.h();
    target.setBottom(clip.y1());
    src.setBottom(src.y1() - src.h() * a);
  }

  return std::make_pair(target, src);
}

void AGPaintProjection::translate(const AGVector2 &v) {
  a.get(2, 0) += v[0];
  a.get(2, 1) += v[1];
}

void AGPaintProjection::transform(const AGMatrix3 &pMatrix) {
  a *= pMatrix;
}

void AGPaintProjection::setClip(const AGRect2&p) {
  clip = clip.intersect(p);
}

AGRect2 AGPaintProjection::getRect() const {
  AGRect2 r = clip;

  r -= AGVector2(a.get(2, 0), a.get(2, 1));
  return r;
}

AGLine2 AGPaintProjection::clipLine(AGLine2 l) const {
  AGLine2 d;

  AGRect2 r = clip;

  if ((l[0][0] < r.x0() && l[1][0] < r.x0()) ||
          (l[0][0] > r.x1() && l[1][0] > r.x1()) ||
          (l[0][1] < r.y0() && l[1][1] < r.y0()) ||
          (l[0][1] > r.y1() && l[1][1] > r.y1()))
    return d; // hidden

  float a;
  // clip
  for (size_t i = 0; i < 1; i++) {
    if (l[i][0] < r.x0()) {
      a = (r.x0() - l[i][0]) / (l[1 - i][0] - l[i][0]);
      l[i][0] = r.x0();
      l[i][1] = l[i][1]+(l[1 - i][1] - l[i][1]) * a;
    }
    if (l[i][1] < r.y0()) {
      a = (r.y0() - l[i][1]) / (l[1 - i][1] - l[i][1]);
      l[i][1] = r.y0();
      l[i][0] = l[i][0]+(l[1 - i][0] - l[i][0]) * a;
    }
    if (l[i][0] > r.x1()) {
      a = (l[i][0] - r.x1()) / (l[i][0] - l[1 - i][0]);
      l[i][0] = r.x1();
      l[i][1] = l[i][1]+(l[1 - i][1] - l[i][1]) * a;
    }
    if (l[i][1] > r.y1()) {
      a = (l[i][1] - r.y1()) / (l[i][1] - l[1 - i][1]);
      l[i][1] = r.y1();
      l[i][0] = l[i][0]+(l[1 - i][0] - l[i][0]) * a;
    }

  }
  return l;

}



/////////////////////////////////////////////////////////////////////////////////
// AGPainter
/////////////////////////////////////////////////////////////////////////////////

AGPainter::AGPainter() : mCurrent(getScreen().getRect()), mTarget(&getScreen()) {
  mTarget->beginPaint();
}

AGPainter::AGPainter(const AGPainter &p) : ps(p.ps), mCurrent(p.mCurrent), mTarget(p.mTarget) {
  CTRACE;
  mTarget->beginPaint();
}

AGPainter::AGPainter(AGPaintTarget &pTarget) : mCurrent(pTarget.getRect()), mTarget(&pTarget) {
  mTarget->beginPaint();
}

AGPainter::~AGPainter() throw () {
  if (mTarget) {
    mTarget->unclip();
    mTarget->endPaint();
  }
}

void AGPainter::putPixel(const AGVector2 &p, const AGColor &c) {
  AGVector2 n = project(p);
  if (pointOk(n)) {
    mTarget->putPixel((int) n[0], (int) n[1], c);
  }
}

void AGPainter::blit(const AGTexture &pSource, const AGRect2 &pDest) {
  AGRect2 s = pSource.getRect();
  // ASSUME: we don't want to scale
  blit(pSource, AGRect2(pDest.x(), pDest.y(), s.w(), s.h()), s);
}

AGVector2 AGPainter::project(const AGVector2 &p) const {
  return mCurrent.project(p);
}

bool AGPainter::pointOk(const AGVector2 &p) const {
  return mCurrent.pointOk(p);
}

void AGPainter::blit(const AGTexture &pSource, const AGRect2 &pDest, const AGRect2 &pSrc) {
  STACKTRACE;
  AGRect2 d;
  d = mCurrent.project(pDest);

#ifdef OLDCLIP

  std::pair<AGRect2, AGRect2> p = mCurrent.clipRect(d, pSrc);

  //  cdebug(p.first<<"   "<<p.second);
  if (p.first.w() > 0 && p.first.h() > 0 && p.second.w() > 0 && p.second.h() > 0)
    mTarget->blit(pSource, p.first, p.second);


#else
#warning "add clipping!"

  mTarget->blit(pSource, d, pSrc); //pSource.getRect());
#endif
}

void AGPainter::blit(const AGTexture &pSource, const AGRect2 &pDest, const AGColor &pColor) {
  AGRect2 s = pSource.getRect();
  // ASSUME: we don't want to scale
  blit(pSource, AGRect2(pDest.x(), pDest.y(), s.w(), s.h()), s, pColor);
}

void AGPainter::blit(const AGTexture &pSource, const AGRect2 &pDest, const AGRect2 &pSrc, const AGColor &pColor) {
  STACKTRACE;
#ifdef SPEED_TEST
  mTarget->blit(pSource, pDest, pSrc, pColor);
  return;
#endif

  AGRect2 d;
  d = mCurrent.project(pDest);
  std::pair<AGRect2, AGRect2> p = mCurrent.clipRect(d, pSrc);

  if (p.first.w() > 0 && p.first.h() > 0 && p.second.w() > 0 && p.second.h() > 0)
    mTarget->blit(pSource, p.first, p.second, pColor);
}

void AGPainter::tile(const AGTexture &pSource) {
  tile(pSource, mTarget->getRect());
}

void AGPainter::tile(const AGTexture &pSource, const AGRect2 &pDest) {
  AGRect2 sourceRect = pSource.getRect();

  tile(pSource, pDest, sourceRect);
}

void AGPainter::tile(const AGTexture &pSource, const AGRect2 &pDest, const AGRect2 &pSrc) {
  STACKTRACE;
  float x, y;
    mTarget->tile(pSource, mCurrent.project(pDest), AGColor(0xff, 0xff, 0xff, 0xff));
}


// AGSurface-painting

void AGPainter::blit(const AGSurface &pSource, const AGRect2 &pDest) {
  AGRect2 s = pSource.getRect();
  // ASSUME: we don't want to scale
  blit(pSource, AGRect2(pDest.x(), pDest.y(), s.w(), s.h()), s);
}

void AGPainter::blit(const AGSurface &pSource, const AGRect2 &pDest, const AGRect2 &pSrc) {
  STACKTRACE;
  AGRect2 d;
  d = mCurrent.project(pDest);
  std::pair<AGRect2, AGRect2> p = mCurrent.clipRect(d, pSrc);

  if (p.first.w() > 0 && p.first.h() > 0 && p.second.w() > 0 && p.second.h() > 0)
    mTarget->blit(pSource, p.first, p.second);
}

void AGPainter::tile(const AGSurface &pSource) {
  tile(pSource, mTarget->getRect());
}

void AGPainter::tile(const AGSurface &pSource, const AGRect2 &pDest) {
  tile(pSource, pDest, pSource.getRect());

}

void AGPainter::tile(const AGSurface &pSource, const AGRect2 &pDest, const AGRect2 &pSrc) {
  STACKTRACE;
  float x, y;

  std::vector<std::pair<AGRect2, AGRect2> > rects;
  for (y = pDest.y0(); y < pDest.y1(); y += pSrc.h())
    for (x = pDest.x0(); x < pDest.x1(); x += pSrc.w()) {
      float w = std::min(pSrc.w(), pDest.x1() - x);
      float h = std::min(pSrc.h(), pDest.y1() - y);

      rects.push_back(std::make_pair(AGRect2(pSrc.x0(), pSrc.y0(), w, h), AGRect2(x, y, w, h)));
    }
  mTarget->blit(pSource, rects, AGColor(0xff, 0xff, 0xff, 0xff));
}

AGColor calcColor(AGVector2 p, const AGColor &pc0, const AGColor &pc1, const AGColor &pc2, const AGColor &pc3) {
  return (pc0 * (1 - p[0]) + pc1 * p[0])*(1 - p[1]) + (pc2 * (1 - p[0]) + pc3 * p[0]) * p[1];
}

void AGPainter::drawGradient(const AGRect2 &pr, const AGColor &pc0, const AGColor &pc1, const AGColor &pc2, const AGColor &pc3) {
  STACKTRACE;
  AGGLScreen *glScreen = dynamic_cast<AGGLScreen*> (mTarget);

  AGRect2 src(0, 0, 1, 1);

  AGRect2 d = mCurrent.project(pr);
  std::pair<AGRect2, AGRect2> p = mCurrent.clipRect(d, src);

  AGColor c0 = calcColor(p.second.getV0(), pc0, pc1, pc2, pc3);
  AGColor c1 = calcColor(p.second.getV10(), pc0, pc1, pc2, pc3);
  AGColor c2 = calcColor(p.second.getV01(), pc0, pc1, pc2, pc3);
  AGColor c3 = calcColor(p.second.getV1(), pc0, pc1, pc2, pc3);

  AGRect2 r = p.first;

  if (p.first.w() > 0 && p.first.h() > 0 && p.second.w() > 0 && p.second.h() > 0) {
    mTarget->drawGradient(r, c0, c1, c2, c3);
  }
}

void AGPainter::renderText(const AGStringUtf8 &pText, const AGVector2 &p, const AGFont &f) {
  STACKTRACE;
  AGTexture *t = getScreen().getFontEngine()->renderText(0, 0, pText, f);
  if (t)
    blit(*t, AGRect2(p[0], p[1], t->getSurfaceWidth(), t->getSurfaceHeight()));

}

void AGPainter::drawBorder(const AGRect2& pRect, int width, const AGColor& c1, const AGColor& c2) {
  STACKTRACE;
  // GL-screen has its own implementation
  AGGLScreen *glScreen = dynamic_cast<AGGLScreen*> (mTarget);
  if (glScreen) {
    glScreen->clip(mCurrent.clip);
    glScreen->drawBorder(mCurrent.project(pRect), width, c1, c2);
    glScreen->unclip();
    return;
  }
  AGRect2 d = pRect;

  for (int t = 0; t < width; t++) {
    drawLine(d.getV0(), d.getV01(), c1);
    drawLine(d.getV0(), d.getV10(), c1);

    drawLine(d.getV01(), d.getV1(), c2);
    drawLine(d.getV10(), d.getV1(), c2);
    d = d.shrink(1);
  }
}

void AGPainter::drawRect(const AGRect2& rect, const AGColor& c) {
  drawLine(rect.getV0(), rect.getV01(), c);
  drawLine(rect.getV0(), rect.getV10(), c);

  drawLine(rect.getV01(), rect.getV1(), c);
  drawLine(rect.getV10(), rect.getV1(), c);
}

AGPaintProjection AGPainter::getCurrentProjection() const {
  return mCurrent;
}

void AGPainter::fillRect(const AGRect2 &pDest, const AGColor &c) {
  STACKTRACE;
  AGRect2 d, pSrc;
  d = mCurrent.project(pDest);
  // FIXME: remove clipRect !!
  AGRect2 p = mCurrent.clipRect(d);
  if (p.w() > 0 && p.h() > 0) {
    //      cdebug(p);
    mTarget->fillRect(p, c);
  }
}

void AGPainter::fillPoly(const std::vector<AGVector2> &pPoly, const AGColor &c) {
  //FIXME: add clipping for none-GL (?) - for performance ?

  std::vector<AGVector2> projected;
  for (std::vector<AGVector2>::const_iterator i = pPoly.begin(); i != pPoly.end(); i++)
    projected.push_back(mCurrent.project(*i));

  mTarget->fillPoly(projected, c);
}

void AGPainter::drawPoly(const std::vector<AGVector2> &pPoly, const AGColor &c) {
  //FIXME: add clipping for none-GL (?) - for performance ?

  std::vector<AGVector2> projected;
  for (std::vector<AGVector2>::const_iterator i = pPoly.begin(); i != pPoly.end(); i++)
    projected.push_back(mCurrent.project(*i));

  mTarget->drawPoly(projected, c);
}

void AGPainter::fillRects(const std::vector<std::pair<AGRect2, AGVector4> > &pRects) {
  STACKTRACE;
  std::vector<std::pair<AGRect2, AGVector4> > rs;

  for (std::vector<std::pair<AGRect2, AGVector4> >::const_iterator i = pRects.begin(); i != pRects.end(); ++i) {
    AGRect2 d = mCurrent.project(i->first);
    if (!opengl())
      d = mCurrent.clipRect(d);
    if (d.w() > 0 && d.h() > 0)
      rs.push_back(std::make_pair(d, i->second));
  }

  if (rs.size())
    mTarget->fillRects(rs);
}

void AGPainter::drawPoint(const AGVector2 &p, const AGColor &c, float size) {
  STACKTRACE;
  AGVector2 d = mCurrent.project(p);
  if (mCurrent.pointOk(p)) {
    mTarget->putPixel((int) d[0], (int) d[1], c);
  }
}

AGColor AGPainter::getPixel(int x, int y) {
  AGVector2 p = mCurrent.project(AGVector2(x, y));
  if (!mCurrent.pointOk(p))
    throw std::string("pixel out of clipping!");
  return mTarget->getPixel((int) p[0], (int) p[1]);
}

void AGPainter::drawLine(const AGVector2 &p0, const AGVector2 &p1, const AGColor &c) {
#warning "Add line clipping"
  AGVector2 m0 = mCurrent.project(p0);
  AGVector2 m1 = mCurrent.project(p1);
  mTarget->drawLine(m0, m1, c);
}

void AGPainter::blitTri(const AGTexture &pSource, const AGTriangle2 &pSrc, const AGTriangle2 &pDest) {
  mTarget->blitTri(pSource, pSrc, pDest);
}

void AGPainter::drawCircle(const AGVector2 &p, float rad, const AGColor &color) {
  // clipping done in putPixel

  int x, y;
  float r2 = rad*rad;
  for (x = ((int) (p[0] - rad - 1)); x <= p[0] + rad + 1; x++)
    for (y = ((int) (p[1] - rad - 1)); y <= p[1] + rad + 1; y++) {
      float dx = x - p[0];
      float dy = y - p[1];
      float d2 = dx * dx + dy*dy;

      if (d2 < r2) {
        AGVector2 c(x, y);
        putPixel(c, color);
      }
    }
}

void AGPainter::setLineWidth(float w) {
  mTarget->setLineWidth(w);
}

void AGPainter::pushMatrix() {
  ps.push_back(mCurrent);
}

void AGPainter::popMatrix() {
  mCurrent = ps.back();
  ps.pop_back();
  mTarget->clip(mCurrent.clip);
}

void AGPainter::translate(const AGVector2 &v) {
  mCurrent.translate(v);
}

void AGPainter::scale(const AGVector2 &v) {
  throw std::string("not implemented!");
}

void AGPainter::clip(const AGRect2 &r) {
  AGRect2 p = mCurrent.project(r);
  mCurrent.setClip(p);
  mTarget->clip(mCurrent.clip);
}

void AGPainter::transform(const AGMatrix3 &m) {
  mCurrent.transform(m);
}

void AGPainter::transform(const AGRect2 &r) {
  translate(r[0]);
  clip(r.origin());
}

AGRect2 AGPainter::getRect() const {
  return mCurrent.getRect();
}

void AGPainter::clip(const AGClipping &clip) {
  CTRACE;
  throw std::runtime_error("NOT IMPLEMENTED !");
}

AGPaintTarget *AGPainter::getTarget() {
  return mTarget;
}

void AGPainter::debugOutput() {
  cdebug(mCurrent.a);
  cdebug(mCurrent.clip);
}
