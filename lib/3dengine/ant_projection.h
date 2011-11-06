#ifndef ANT_PROJECTION_H
#define ANT_PROJECTION_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "ag_gl.h"
#include <ant_frustum.h>

struct AGEXPORT Viewport
{
  GLint viewport[4];
#ifndef SWIG
  (operator GLint *)() const
  {
    return const_cast<Viewport*>(this)->viewport;
  }
#endif
};


class AGEXPORT AntProjection
{
 public:
  AntProjection();
  AntProjection(const AGMatrix4 &pMv,const AGMatrix4 &pPr,const Viewport &pVp);

  AGVector3 project(const AGVector3 &p) const;
  AGVector3 unProject(const AGVector3 &p) const;

  AntFrustum getFrustum() const;

 private:
  AGMatrix4 modelview,projection;
  Viewport viewport;
};

#endif
