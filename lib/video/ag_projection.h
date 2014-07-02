#ifndef AG_PROJECTION
#define AG_PROJECTION

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_base.h>
#include <ag_geometry.h>


class AGEXPORT AGProjection2D
  {
public:
  AGProjection2D();
  AGProjection2D(const AGProjection2D &p);
  AGProjection2D(const AGRect2 &from, const AGRect2 &to);
  AGProjection2D(const AGMatrix3 &pMatrix) throw (GeometryException);

  AGRect2 project(const AGRect2 &r) const;
  AGVector2 project(const AGVector2 &p) const;
  AGVector3 project(const AGVector3 &p) const;
  AGTriangle2 project(const AGTriangle2 &t) const;
  AGTriangle3 project(const AGTriangle3 &t) const;

  AGProjection2D inverse() const throw (GeometryException);

  void pushProjection(const AGProjection2D &p);

  AGMatrix3 getMatrix() const;
private:
  bool mInited;
  AGMatrix3 m;
  };




#endif
