#include "math_algebra.h"

#include "rk_debug.h"

#include "ag_gauss.h"
#include "ag_stringstream.h"

// convert from AGMatrix4
AGMatrixN::AGMatrixN(const AGMatrix4 &p):m(4*4,0),
mW(4),mH(4)
  {
    for(size_t x=0;x<4;x++)
      for(size_t y=0;y<4;y++)
        {
          set(x,y,p.get(x,y));
        }
  }

// another dummy commit (3)
AGMatrixN::AGMatrixN(const AGMatrix3 &p):m(3*3,0),
mW(3),mH(3)
  {
    for(size_t x=0;x<3;x++)
      for(size_t y=0;y<3;y++)
        {
          set(x,y,p.get(x,y));
        }
  }


AGMatrixN::AGMatrixN(size_t w,size_t h):m(w*h,0),
mW(w),mH(h)
  {
  }

AGMatrixN::AGMatrixN(const AGMatrixN &n):m(n.m),
mW(n.mW),
mH(n.mH)
  {
  }

void AGMatrixN::set(size_t x,size_t y,float v)
  {
    assert(x<mW);
    assert(y<mH);
    m[index(x,y)]=v;
  }
float AGMatrixN::get(size_t x,size_t y) const
{
  assert(x<mW);
  assert(y<mH);
  return m[index(x,y)];
}

AGMatrixN &AGMatrixN::operator*=(const AGMatrixN &p)
  {
    assert(mW==p.mH);
    AGMatrixN n(p.mW,mH);
    for(size_t x=0;x<p.mW;x++)
      for(size_t y=0;y<mH;y++)
        {
          float v=0;
          for(size_t k=0;k<mW;k++)
            v+=get(k,y)*p.get(x,k);
          n.set(x,y,v);
        }
    *this=n;
    return *this;
  }


AGMatrixN &AGMatrixN::operator+=(const AGMatrixN &p)
  {
    assert(mW==p.mW && mH==p.mH);
    for(size_t x=0;x<mW;x++)
      for(size_t y=0;y<mH;y++)
        {
          set(x,y,get(x,y)+p.get(x,y));
        }
    return *this;
  }

AGMatrixN AGMatrixN::operator*(const AGMatrixN &p) const
{
  assert(mW==p.mH);
  AGMatrixN n(p.mW,mH);
  for(size_t x=0;x<p.mW;x++)
    for(size_t y=0;y<mH;y++)
      {
        float v=0;
        for(size_t k=0;k<mW;k++)
          v+=get(k,y)*p.get(x,k);
        n.set(x,y,v);
      }
  return n;
}

AGMatrixN AGMatrixN::operator*(float f) const
{
  AGMatrixN n(mW,mH);
  for(size_t x=0;x<mW;x++)
    for(size_t y=0;y<mH;y++)
      n.set(x,y,get(x,y)*f);
  return n;
}


AGMatrixN AGMatrixN::operator-(const AGMatrixN &p) const
{
  assert(mW==p.mW);
  assert(mH==p.mH);
  AGMatrixN n(mW,mH);

  for(size_t x=0;x<mW;x++)
    for(size_t y=0;y<mH;y++)
      n.set(x,y,get(x,y)-p.get(x,y));
  return n;
}

float AGMatrixN::scalar() const
{
  float s=0;
  float v;
  for(size_t x=0;x<mW;x++)
    for(size_t y=0;y<mH;y++)
      {
        v=get(x,y);
        s+=v*v;
      }
  return sqrt(s);
}


AGMatrixN AGMatrixN::makeQuadratic() const
{
  size_t nw=std::max(mW,mH);
  AGMatrixN a(nw,nw);
  a.copyFrom(*this);
  return a;
}

/// a very simple way to compute a pseudo-inverse
/// using svd may be better - no matter ;-)
/// for details go to http://en.wikipedia.org/wiki/Pseudoinverse
AGMatrixN AGMatrixN::pseudoInverse() const
{
  AGMatrixN t=transposed();
  if(mW<mH)
    return (t*(*this)).inverse()*t;
  else
    return t*((*this)*t).inverse();
}

AGMatrixN AGMatrixN::transposed() const
{
  AGMatrixN a(mH,mW);
  for(size_t x=0;x<mW;x++)
    for(size_t y=0;y<mH;y++)
      a.set(y,x,get(x,y));
  return a;
}



AGMatrixN AGMatrixN::inverse() const
{
  size_t nw=std::max(mW,mH);
  AGMatrixN a(nw,nw),b(nw,nw);

  a.makeUnitMatrix();
  b.copyFrom(*this);
  gauss(a,b,nw);
  return a;
}

void AGMatrixN::makeUnitMatrix()
  {
    for(size_t x=0;x<mW;x++)
      for(size_t y=0;y<mH;y++)
        set(x,y,(x==y?1:0));
  }

void AGMatrixN::copyFrom(const AGMatrixN &p)
  {
    for(size_t x=0;x<mW && x<p.mW;x++)
      for(size_t y=0;y<mH && y<p.mH;y++)
        set(x,y,p.get(x,y));
  }

void AGMatrixN::output() const
{
  for(size_t y=0;y<mH;y++)
    {
      for(size_t x=0;x<mW;x++)
        {
          std::cout<<get(x,y)<<"\t";
        }
      std::cout<<"\n";
    }

}

AGString AGMatrixN::toString() const
{
  AGStringStream s;
  for(size_t y=0;y<mH;y++)
    {
      for(size_t x=0;x<mW;x++)
        {
          s<<get(x,y)<<"\t";
        }
      s<<"\n";
    }
  return s.str();
}

void AGMatrixN::swapRows(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<mH);
    assert(b<mH);
    for(size_t x=0;x<mW;x++)
      {
        float t=get(x,a);
        set(x,a,get(x,b));
        set(x,b,t);
      }
  }
void AGMatrixN::swapCols(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<mW);
    assert(b<mW);
    for(size_t y=0;y<mH;y++)
      {
        float t=get(a,y);
        set(a,y,get(b,y));
        set(b,y,t);
      }
  }

size_t AGMatrixN::width() const
{
  return mW;
}
size_t AGMatrixN::height() const
{
  return mH;
}

AGMatrixN AGMatrixN::skipRow(size_t i) const
{
  assert(height()>1);
  assert(i<height());
  AGMatrixN n(width(),height()-1);

  for(size_t x=0;x<width();x++)
    {
      for(size_t y=0;y<i;y++)
        n.set(x,y,get(x,y));
      for(size_t y=i+1;y<height();y++)
        n.set(x,y-1,get(x,y));
    }
  return n;
}
AGMatrixN AGMatrixN::skipCol(size_t i) const
{
  assert(width()>1);
  assert(i<width());
  AGMatrixN n(width()-1,height());

  for(size_t y=0;y<height();y++)
    {
      for(size_t x=0;x<i;x++)
        n.set(x,y,get(x,y));
      for(size_t x=i+1;x<width();x++)
        n.set(x-1,y,get(x,y));
    }
  return n;
}


template<class T> float determinant(const T&t)
  {
    // FIXME: det for non-quad matrices ???
    assert(t.width()==t.height());
    if(t.width()>2)
      {
        AGMatrixN n(t.width()-1,t.height()-1);
        AGMatrixN l(t.skipCol(0));
        assert(l.width()==t.height()-1);
        float c;
        // take always first row for reduction
        for(size_t i=0;i<t.height();i++)
          {
            c=(i&1)?-1:1;
            n+=l.skipRow(i)*t.get(0,i)*c;
          }
        return determinant(n);
      }
    else
      {
        assert(t.width()==2);
        assert(t.height()==2);
        return t.get(0,0)*t.get(1,1)-t.get(1,0)*t.get(0,1);
      }
  }

template float determinant<AGMatrixN>(const AGMatrixN&p);
