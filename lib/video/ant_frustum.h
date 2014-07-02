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

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef ANT_FRUSTUM_H
#define ANT_FRUSTUM_H

#include "ag_geometry.h"

class AGEXPORT AntPlane
{
 public:
  AntPlane(const AGVector3 &dir,float offset);

  bool inside(const AGVector3 &v) const;

  AGString toString() const;
 private:
  AGVector3 mDir;
  float mOffset;
};

/// p0 is center - p1 is above and p2 at the right, then positive normal points into screen
AGEXPORT AntPlane makePlane(const AGVector3 &p0,const AGVector3 &p1,const AGVector3 &p2);

class AGEXPORT AntFrustum
{
 public:
  AntFrustum();
  AntFrustum(const std::vector<AntPlane> &pPlanes);

  bool inside(const AGVector3 &v) const;
  bool collides(const AGBox3 &v) const;

  AGString toString() const;
 private:
  std::vector<AntPlane> mPlanes;
};

AGEXPORT std::ostream &operator<<(std::ostream &o,const AntPlane &p);
AGEXPORT std::ostream &operator<<(std::ostream &o,const AntFrustum &p);

inline bool AntPlane::inside(const AGVector3 &v) const
{
  return v*mDir-mOffset>0;
}

#endif
