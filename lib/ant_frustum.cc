/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ant_frustum.h
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

#include "ant_frustum.h"
#include "rk_debug.h"
#include "ag_profiler.h"

/////////////////////////////////////////////////////////////////////
// AntPlane
/////////////////////////////////////////////////////////////////////

AntPlane::AntPlane(const AGVector3 &dir, float offset) :
mDir(dir),
mOffset(offset) {
}

AntPlane makePlane(const AGVector3 &p0, const AGVector3 &p1, const AGVector3 &p2) {
  AGVector3 up = p1 - p0;
  AGVector3 right = p2 - p0;
  up /= up.length();
  right /= right.length();
#warning "maybe this must be swapped!"
  AGVector3 dir = up % right;

  return AntPlane(dir, dir * p0);
}

AGString AntPlane::toString() const {
  std::ostringstream os;
  os << "(" << mDir << "," << mOffset << ")";
  return os.str();
}

/////////////////////////////////////////////////////////////////////
// AntFrustum
/////////////////////////////////////////////////////////////////////

AntFrustum::AntFrustum() {
  throw std::runtime_error("Possible error in AntFrustum::AntFrustum(): this function shouldn't be called!");
}

AntFrustum::AntFrustum(const std::vector<AntPlane> &pPlanes) :
mPlanes(pPlanes) {
  assert(mPlanes.size() == 6);

  //  cdebug(*this);
}

bool AntFrustum::inside(const AGVector3 &v) const {
  for (std::vector<AntPlane>::const_iterator i = mPlanes.begin(); i != mPlanes.end(); ++i)
    if (!i->inside(v))
      return false;
  return true;
}

bool AntFrustum::collides(const AGBox3 &b) const {
  static std::vector<AGVector3> vs(8);
  {
    b.calcVertices(vs);
  }
  static bool outside;
  static std::vector<AntPlane>::const_iterator i;
  static std::vector<AGVector3>::iterator j;


  for (i = mPlanes.begin(); i != mPlanes.end(); ++i) {
    outside = true;
    for (j = vs.begin(); j != vs.end(); ++j) {
      if (i->inside(*j)) {
        outside = false;
        break;
      }
    }
    if (outside)
      return false;
  }
  return true;
}

AGString AntFrustum::toString() const {
  std::ostringstream os;
  for (std::vector<AntPlane>::const_iterator i = mPlanes.begin(); i != mPlanes.end(); ++i)
    os << *i << ";";
  return os.str();


}

std::ostream & operator<<(std::ostream &o, const AntPlane &p) {
  o << p.toString();
  return o;
}

std::ostream & operator<<(std::ostream &o, const AntFrustum &p) {
  o<<p;
  return o;
}
