#include "ag_projection.h"
#include "rk_debug.h"
#include "ag_algebra.h"

AGProjection2D::AGProjection2D() :
  mInited(false)
  {
  }

AGProjection2D::AGProjection2D(const AGMatrix3 &pMatrix) throw (GeometryException) :
  mInited(true), m(pMatrix)
  {
    bool invertable=isInvertable(m);
    if(!invertable)
      {
        throw GeometryException(AGString("Matrix ")+m.toString()+" is not invertible");
      }
    assert(invertable);
  }

AGProjection2D::AGProjection2D(const AGProjection2D &p) :
  mInited(p.mInited), m(p.m)
  {
  }

AGProjection2D::AGProjection2D(const AGRect2 &from, const AGRect2 &to) :
  mInited(true)
  {
    assert(from.content()>0 && to.content()>0);

    float sx=to.w()/from.w();
    float sy=to.h()/from.h();
    AGMatrix3 m1(-from.getV0());
    AGMatrix3 m2(sx, sy);
    AGMatrix3 m3(to.getV0());

    m=m3*m2*m1;

    assert(isInvertable(m));
  }

AGRect2 AGProjection2D::project(const AGRect2 &r) const
{
  assert(mInited);
  return AGRect2((m*r.getV0()).dim2(), (m*r.getV1()).dim2());
}
AGVector2 AGProjection2D::project(const AGVector2 &p) const
{
  assert(mInited);
  return (m*p).dim2();
}

/// leaves 3rd dimension untouched
AGVector3 AGProjection2D::project(const AGVector3 &p) const
{
  assert(mInited);
  return AGVector3((m*p.dim2()).dim2(),p[2]);
}

AGTriangle2 AGProjection2D::project(const AGTriangle2 &t) const
{
  return AGTriangle2(project(t.get(0)),project(t.get(1)),project(t.get(2)));
}
// leaves 3rd dimension untouched
AGTriangle3 AGProjection2D::project(const AGTriangle3 &t) const
{
  return AGTriangle3(project(t[0]),project(t[1]),project(t[2]));
}


void AGProjection2D::pushProjection(const AGProjection2D &p)
  {
    assert(mInited);
    m*=p.m;
  }

AGProjection2D AGProjection2D::inverse() const throw (GeometryException)
  {
    CTRACE;
    bool invertable=isInvertable(m);
    if(!invertable)
      {
        cdebug("Matrix:"<<m);
        throw GeometryException(AGString("Matrix ")+m.toString()+" is not invertible");
      }
    AGMatrix3 inv=m.inverted();
    return AGProjection2D(inv);
  }

AGMatrix3 AGProjection2D::getMatrix() const
{
  return m;
}
