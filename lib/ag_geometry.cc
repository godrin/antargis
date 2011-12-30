/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_triangle.cc
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

#include <ag_geometry.h>
#include <rk_tools.h>
#include <rk_debug.h>

#include <assert.h>
#include <cmath>
#include <map>
#include <list>
#include <ag_xml.h>
#include <ag_stringstream.h>
#include <ag_gauss.h>


#define MAXF 100000.0f

AGVector2 invalidVec2;

bool collide1d(float a1,float a2,float b1,float b2, bool normal=true)
  {
    float amin=std::min(a1,a2);
    float amax=std::max(a1,a2);
    float bmin=std::min(b1,b2);
    float bmax=std::max(b1,b2);

    if(normal && (amax==bmin || bmax==amin))
      return false;

    return (amin>=bmin && amin<=bmax) || (amax>=bmin && amax<=bmax) || (bmin>=amin && bmin<=amax) || (bmax>=amin && bmax<=amin);
  }

struct AGSweepResult
  {
    enum CollisionState { COL_AT, COL_ALWAYS, COL_NEVER };
    CollisionState state;
    float t0;
    float t1;

    AGSweepResult()
      {
        state=COL_ALWAYS;
      }

    void combine(const AGSweepResult &r)
      {
        if(state==COL_AT)
          {
            if(r.state==COL_NEVER)
              state=COL_NEVER;
            else if(r.state!=COL_ALWAYS) // ALWAYS has no effect
              {
                t0=std::max(t0,r.t0);
                t1=std::min(t1,r.t1);
              }
          }
        else if(state==COL_ALWAYS)
          {
            if(r.state==COL_NEVER)
              state=COL_NEVER;
            else if(r.state!=COL_ALWAYS) // ALWAYS has no effect
              {
                state=COL_AT;
                t0=r.t0;
                t1=r.t1;
              }

          }
        // COL_NEVER cannot be changed
      }

    bool collision(float delta) const
    {
      return state==COL_ALWAYS || (state==COL_AT && t1>0 && t0<delta);
    }

    float begin(float delta) const
    {
      if(state==COL_ALWAYS || state==COL_NEVER)
        return MAXF;

      if(t0>=0.0f && t0<delta)
        return t0;
      else
        return MAXF;
    }

    bool always() const
    {
      return state == COL_ALWAYS;
    }
  };

AGSweepResult collide1d(float pa1,float pa2,float pb1,float pb2,float v)
  {
    float a1=std::min(pa1,pa2);
    float a2=std::max(pa1,pa2);
    float b1=std::min(pb1,pb2);
    float b2=std::max(pb1,pb2);
    AGSweepResult r;

    if(v>0)
      {
        r.t0=(b1-a2)/v;
        r.t1=(b2-a1)/v;
        r.state = AGSweepResult::COL_AT;

        assert(r.t0 <= r.t1);
      }
    else if(v<0)
      {
        r.t0=(b2-a1)/v;
        r.t1=(b1-a2)/v;
        r.state = AGSweepResult::COL_AT;

        assert(r.t0 <= r.t1);
      }
    else
      {
        if (a2 < b1 || a1 > b2)
          r.state = AGSweepResult::COL_NEVER;
        else
          r.state = AGSweepResult::COL_ALWAYS;
      }
    return r;
  }

/////////////////////////////////////////////////////////////////////////////
// AGAngle
/////////////////////////////////////////////////////////////////////////////

AGAngle::AGAngle(float p):angle(p)
      {
      }

/////////////////////////////////////////////////////////////////////////////
// AGVector2
/////////////////////////////////////////////////////////////////////////////

AGVector2::AGVector2(const AGString &s)
  {
    assert(s.length()>=5);
    assert(s[0]=='(');
    assert(s[s.length()-1]==')');
    AGString p=s.substr(1,s.length()-2);
    size_t i=p.find(",");
    assert(i!=p.npos);
    v[0]=p.substr(0,i).toFloat();
    v[1]=p.substr(i+1,p.length()-i-1).toFloat();
  }


AGVector2::AGVector2(float pX,float pY)
  {
    v[0]=pX;
    v[1]=pY;
  }
AGVector2::AGVector2(const AGVector2 &a)
  {
    v[0]=a.v[0];
    v[1]=a.v[1];
  }
AGVector2::AGVector2(const AGAngle &a)
  {
    v[0]=sin(a.angle);
    v[1]=-cos(a.angle);
  }

AGVector2::AGVector2()
  {
    v[0]=v[1]=0;
  }

void AGVector2::saveXML(Node &node) const
{
  node.set("x",AGString(v[0]));
  node.set("y",AGString(v[1]));
}
void AGVector2::loadXML(const Node &node)
  {
    v[0]=node.get("x").toFloat();
    v[1]=node.get("y").toFloat();
  }

bool AGVector2::operator<(const AGVector2 &p) const
{
  return v[0]<p.v[0] || (v[0]==p.v[0] && v[1]<p.v[1]);
}


AGVector2 AGVector2::operator-() const
{
  return AGVector2(-v[0],-v[1]);
}


float getArcInternal(float x,float y)
  {
    if(y==0.0)
      {
        if(x<0.0)
          return -M_PI/2.0;
        else
          return M_PI/2.0;
      }
    else if(y<0.0)
      {
        float a=M_PI+atan(x/y);
        if(a>M_PI)
          a-=M_PI*2.0;
        return a;
      }
    else
      return atan(x/y);
  }


AGAngle AGVector2::getAngle() const
{
  return AGAngle(getArcInternal(v[0],v[1]));
}


bool AGVector2::operator==(const AGVector2 &a) const
{
  return v[0]==a.v[0] && v[1]==a.v[1];// && v[2]==a.v[2];
}
bool AGVector2::operator!=(const AGVector2 &a) const
{
  return !operator==(a);
}

AGString AGVector2::toString() const
{
  std::ostringstream os;
  os.str("");
  os<<"("<<v[0]<<","<<v[1]<<")";
  return AGString(os.str());
}

float AGVector2::length2() const
{
  return v[0]*v[0]+v[1]*v[1];
}

AGVector2 AGVector2::dim2() const
{
  return *this;
}


bool AGVector2::nonZero() const
{
  return length2()!=0.0f;
}

AGVector2 AGVector2::normal() const
{
  return AGVector2(-v[1],v[0]);
}

void AGVector2::setX(float pX)
  {
    v[0]=pX;
  }
void AGVector2::setY(float pY)
  {
    v[1]=pY;
  }

float AGVector2::getX() const
{
  return v[0];
}
float AGVector2::getY() const
{
  return v[1];
}

(AGVector2::operator float*)()
      {
        return v;
      }
(AGVector2::operator const float*)() const
{
  return v;
}

AGVector2 AGVector2::operator-(const AGVector2 &p) const
{
  return AGVector2(v[0]-p.v[0],v[1]-p.v[1]);
}
AGVector2 AGVector2::operator+(const AGVector2 &p) const
{
  return AGVector2(v[0]+p.v[0],v[1]+p.v[1]);
}
AGVector2 &AGVector2::operator+=(const AGVector2 &p)
      {
        v[0]+=p.v[0];
        v[1]+=p.v[1];
        return *this;
      }
AGVector2 &AGVector2::operator-=(const AGVector2 &p)
      {
        v[0]-=p.v[0];
        v[1]-=p.v[1];
        return *this;
      }

float AGVector2::operator*(const AGVector2 &p) const
{
  return v[0]*p.v[0]+v[1]*p.v[1];
}
AGVector2 &AGVector2::operator*=(float f)
      {
        v[0]*=f;
        v[1]*=f;
        return *this;
      }
AGVector2 &AGVector2::operator/=(float f)
      {
        v[0]/=f;
        v[1]/=f;
        return *this;
      }
AGVector2 AGVector2::operator*(float f) const
{
  return AGVector2(v[0]*f,v[1]*f);
}
AGVector2 AGVector2::operator/(float f) const
{
  f=1.0/f;
  return AGVector2(v[0]*f,v[1]*f);
}

float AGVector2::length() const
{
  return sqrt(v[0]*v[0]+v[1]*v[1]);
}

AGVector2 AGVector2::normalized() const
{
  if(length2()!=0.0f)
    return *this/length();
  else
    return *this;
}
void AGVector2::normalize()
  {
    if(length2()!=0.0f)
      operator/=(length());
  }

float AGVector2::operator[](int index) const
{
  assert(index>=0 && index<2);
  return v[index];
}
float &AGVector2::operator[](int index)
      {
        assert(index>=0 && index<2);
        return v[index];
      }


/////////////////////////////////////////////////////////////////////////////
// AGVector3
/////////////////////////////////////////////////////////////////////////////

AGVector3::AGVector3(const AGVector2 &p,float h)
  {
    v[0]=p[0];
    v[1]=p[1];
    v[2]=h;
  }

AGVector3::AGVector3(float pX,float pY,float pZ)
  {
    v[0]=pX;
    v[1]=pY;
    v[2]=pZ;
  }
AGVector3::AGVector3(const AGVector3 &a)
  {
    v[0]=a.v[0];
    v[1]=a.v[1];
    v[2]=a.v[2];
  }
AGVector3::AGVector3(const AGAngle &a)
  {
    v[0]=sin(a.angle);
    v[1]=-cos(a.angle);
    v[2]=0.0f;
  }

AGVector3::AGVector3()
  {
    v[0]=v[1]=0;
    v[2]=0.0f;
  }

AGVector2 AGVector3::dim2() const
{
  return AGVector2(v[0],v[1]);
}


AGVector3 AGVector3::operator-() const
{
  return AGVector3(-v[0],-v[1],-v[2]);
}


void AGVector3::saveXML(Node &node) const
{
  node.set("x",AGString(v[0]));
  node.set("y",AGString(v[1]));
  node.set("z",AGString(v[2]));
}
void AGVector3::loadXML(const Node &node)
  {
    v[0]=node.get("x").toFloat();
    v[1]=node.get("y").toFloat();
    v[2]=node.get("z").toFloat();
  }




/*
float getArcInternal(float x,float y)
{
  if(y==0.0)
    {
      if(x<0.0)
        return -M_PI/2.0;
      else
        return M_PI/2.0;
    }
  else if(y<0.0)
    {
      float a=M_PI+atan(x/y);
      if(a>M_PI)
        a-=M_PI*2.0;
      return a;
    }
  else
    return atan(x/y);
}
 */

AGAngle AGVector3::getAngle() const
{
  return AGAngle(getArcInternal(v[0],v[1]));
}


bool AGVector3::operator==(const AGVector3 &a) const
{
  float m=std::max(v[0],std::max(v[1],v[2]));

  m*=0.0001;
  return fabs(v[0]-a.v[0])<m && fabs(v[1]-a.v[1])<m && fabs(v[2]-a.v[2])<m;
}
bool AGVector3::operator!=(const AGVector3 &a) const
{
  return !operator==(a);
}

AGString AGVector3::toString() const
{
  std::ostringstream os;
  os<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")";
  return AGString(os.str());
}

float AGVector3::length2() const
{
  return v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}


bool AGVector3::nonZero() const
{
  return length2()!=0.0f;
}

AGVector3 AGVector3::normal() const
{
  return AGVector3(-v[1],v[0],v[2]);
}

void AGVector3::setX(float pX)
  {
    v[0]=pX;
  }
void AGVector3::setY(float pY)
  {
    v[1]=pY;
  }
void AGVector3::setZ(float pZ)
  {
    v[2]=pZ;
  }

float AGVector3::getX() const
{
  return v[0];
}
float AGVector3::getY() const
{
  return v[1];
}

float AGVector3::getZ() const
{
  return v[2];
}

(AGVector3::operator float*)()
      {
        return v;
      }
AGVector3 AGVector3::operator%(const AGVector3 &a) const
{
  return AGVector3(v[1] * a.v[2] - v[2] * a.v[1],
      v[2] * a.v[0] - v[0] * a.v[2],
      v[0] * a.v[1] - v[1] * a.v[0]);
}


AGVector3 AGVector3::operator-(const AGVector3 &p) const
{
  return AGVector3(v[0]-p.v[0],v[1]-p.v[1],v[2]-p.v[2]);
}
AGVector3 AGVector3::operator+(const AGVector3 &p) const
{
  return AGVector3(v[0]+p.v[0],v[1]+p.v[1],v[2]+p.v[2]);
}
AGVector3 &AGVector3::operator+=(const AGVector3 &p)
      {
        v[0]+=p.v[0];
        v[1]+=p.v[1];
        v[2]+=p.v[2];
        return *this;
      }
AGVector3 &AGVector3::operator-=(const AGVector3 &p)
      {
        v[0]+=p.v[0];
        v[1]+=p.v[1];
        v[2]+=p.v[2];
        return *this;
      }

AGVector3 &AGVector3::operator*=(float f)
      {
        v[0]*=f;
        v[1]*=f;
        v[2]*=f;
        return *this;
      }
AGVector3 &AGVector3::operator/=(float f)
      {
        v[0]/=f;
        v[1]/=f;
        v[2]/=f;
        return *this;
      }
AGVector3 AGVector3::operator*(float f) const
{
  return AGVector3(v[0]*f,v[1]*f,v[2]*f);
}
AGVector3 AGVector3::operator/(float f) const
{
  f=1.0/f;
  return AGVector3(v[0]*f,v[1]*f,v[2]*f);
}

float AGVector3::length() const
{
  return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

AGVector3 AGVector3::normalized() const
{
  if(length2()!=0.0f)
    return *this/length();
  else
    return *this;
}
void AGVector3::normalize()
  {
    if(length2()!=0.0f)
      operator/=(length());
  }

float AGVector3::operator[](int index) const
{
  assert(index>=0 && index<3);
  return v[index];
}
float &AGVector3::operator[](int index)
      {
        assert(index>=0 && index<3);
        return v[index];
      }


////////////////////////////////////////////////////////////////////////////
// AGMatrix
/////////////////////////////////////////////////////////////////////////////

AGMatrix3::AGMatrix3()
  {
    a[0][1]=a[0][2]=
      a[1][0]=a[1][2]=
        a[2][0]=a[2][1]=0.0f;

    a[0][0]=a[1][1]=a[2][2]=1.0f;
  }

AGMatrix3::AGMatrix3(const AGAngle &n)
  {
    a[0][0]=cos(n.angle);
    a[1][0]=sin(n.angle);
    a[0][1]=-sin(n.angle);
    a[1][1]=cos(n.angle);
    a[2][0]=a[2][1]=a[0][2]=a[1][2]=0.0f;
    a[2][2]=1.0f;
  }

AGMatrix3::AGMatrix3(const AGVector3 &n)
  {
    a[0][0]=a[1][1]=a[2][2]=1.0f;
    a[0][1]=a[0][2]=
      a[1][0]=a[1][2]=
        a[2][0]=a[2][1]=0.0f;
    a[2][0]=n[0];
    a[2][1]=n[1];
  }

AGMatrix3::AGMatrix3(float x,float y)
  {
    a[0][0]=x;
    a[1][1]=y;
    a[2][2]=1.0f;
    a[0][1]=a[0][2]=
      a[1][0]=a[1][2]=
        a[2][0]=a[2][1]=0.0f;
    a[2][0]=0.0f;
    a[2][1]=0.0f;

  }
AGMatrix3::AGMatrix3(const AGMatrix3 &m)
  {
    for(size_t x=0;x<3;x++)
      for(size_t y=0;y<3;y++)
        a[x][y]=m.a[x][y];
  }


AGMatrix3 AGMatrix3::transposed() const
{
  AGMatrix3 a;
  for(size_t x=0;x<3;x++)
    for(size_t y=0;y<3;y++)
      a.set(x,y,get(y,x));
  return a;
}


AGMatrix3 AGMatrix3::inverted() const throw(GeometryException)
    {
      // gauss-alg.
      AGMatrix3 a;
      AGMatrix3 b(*this);
      try
      {
        gauss(a,b,3);
      }
      catch(...)
      {
        throw GeometryException("Matrix is not invertable");
      }
      return a;
    }

void AGMatrix3::swapRows(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<3);
    assert(b<3);
    for(size_t x=0;x<3;x++)
      {
        float t=get(x,a);
        set(x,a,get(x,b));
        set(x,b,t);
      }
  }

void AGMatrix3::swapCols(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<3);
    assert(b<3);
    for(size_t y=0;y<3;y++)
      {
        float t=get(a,y);
        set(a,y,get(b,y));
        set(b,y,t);
      }
  }


void AGMatrix3::set(size_t x,size_t y,float f)
  {
    assert(x>=0 && x<3);
    assert(y>=0 && y<3);
    a[x][y]=f;
  }
float AGMatrix3::get(size_t x,size_t y) const
{
  assert(x>=0 && x<3);
  assert(y>=0 && y<3);
  return a[x][y];
}

float &AGMatrix3::get(size_t x,size_t y)
  {
    assert(x>=0 && x<3);
    assert(y>=0 && y<3);
    return a[x][y];
  }

bool AGMatrix3::operator==(const AGMatrix3&p) const
{
  for(size_t x=0;x<3;x++)
    for(size_t y=0;y<3;y++)
      {
        if(fabs(get(x,y)-p.get(x,y))>0.0001)
          return false;
      }
  return true;
}


AGMatrix3::Row AGMatrix3::operator[](size_t y)
      {
        Row r;
        r.matrix=this;
        r.y=y;
        return r;
      }
const AGMatrix3::Row AGMatrix3::operator[](size_t y) const
{
  Row r;
  r.matrix=const_cast<AGMatrix3*>(this);
  r.y=y;
  return r;
}

AGMatrix3 AGMatrix3::operator-() const
{
  AGMatrix3 n;
  for(size_t x=0;x<3;x++)
    for(size_t y=0;y<3;y++)
      n[x][y]=-(*this)[x][y];
  return n;
}



AGMatrix3 AGMatrix3::operator*(const AGMatrix3 &m) const
{
  AGMatrix3 n;
  size_t x,y;
  for(x=0;x<3;x++)
    for(y=0;y<3;y++)
      n.a[x][y]=a[0][y]*m.a[x][0]+
      a[1][y]*m.a[x][1]+
      a[2][y]*m.a[x][2];
  return n;
}
AGMatrix3 &AGMatrix3::operator*=(const AGMatrix3 &m)
      {
        *this=(*this)*m;
        return *this;
      }

AGVector3 AGMatrix3::operator*(const AGVector3 &v) const
{
  return AGVector3(a[0][0]*v[0]+a[1][0]*v[1]+a[2][0]*v[2],
      a[0][1]*v[0]+a[1][1]*v[1]+a[2][1]*v[2],
      a[0][2]*v[0]+a[1][2]*v[1]+a[2][2]*v[2]);
}

AGString AGMatrix3::toString() const
{
  std::ostringstream os;
  os<<"("<<a[0][0]<<","<<a[1][0]<<","<<a[2][0]<<")"<<std::endl;
  os<<"("<<a[0][1]<<","<<a[1][1]<<","<<a[2][1]<<")"<<std::endl;
  os<<"("<<a[0][2]<<","<<a[1][2]<<","<<a[2][2]<<")"<<std::endl;
  return AGString(os.str());
}

/////////////////////////////////////////////////////////////////////////////
// AGTriangle2
/////////////////////////////////////////////////////////////////////////////

AGTriangle2::AGTriangle2()
  {

  }

AGTriangle2::AGTriangle2(const AGVector2 &v0,const AGVector2 &v1,const AGVector2 &v2)
  {
    p[0]=v0;
    p[1]=v1;
    p[2]=v2;
  }

void AGTriangle2::flip()
  {
    AGVector2 tmp=p[2];
    p[2]=p[1];
    p[1]=tmp;
  }

AGRect2 AGTriangle2::getBBox() const
{
  float minx=std::min(p[0].getX(),std::min(p[1].getX(),p[2].getX()));
  float maxx=std::max(p[0].getX(),std::max(p[1].getX(),p[2].getX()));
  float miny=std::min(p[0].getY(),std::min(p[1].getY(),p[2].getY()));
  float maxy=std::max(p[0].getY(),std::max(p[1].getY(),p[2].getY()));

  return AGRect2(minx,miny,maxx-minx,maxy-miny);
}


void AGTriangle2::apply(const AGMatrix3 &m)
  {
    p[0]=(m*AGVector3(p[0],1)).dim2();
    p[1]=(m*AGVector3(p[1],1)).dim2();
    p[2]=(m*AGVector3(p[2],1)).dim2();
  }

/* FIXME: this will be some sweep-base collision detection,
   HOWEVER there's the angular-velocity still missing :-(

AGCollisionData AGTriangle2::collide(const AGTriangle2 &t,const AGVector &v0,const AGVector &v1) const
{
  AGCollisionData data;

  AGVector v=v1-v0;

  float t0,t1;

  std::vector<AGVector> v=getNormals();
  append(v,t.getNormals());
  std::vector<AGVector>::iterator i=v.begin();
  for(;i!=v.end();i++)
    {
      float min1,min2,max1,max2;
      size_t j;
      for(j=0;j<3;j++)
      {
        if(j==0)
          {
            min1=max1=(*i)*p[j];
            min2=max2=(*i)*t.p[j];
          }
        else
          {
            float v0=(*i)*p[j];
            float v1=(*i)*t.p[j];
            min1=std::min(min1,v0);
            max1=std::max(max1,v0);
            min2=std::min(min2,v1);
            max2=std::max(max2,v1);
          }
      }

      float a1=
    }
}
 */

std::vector<AGVector2> AGTriangle2::collisionPoints(const AGLine2 &l) const
{
  std::vector<AGVector2> ps;
  std::vector<AGLine2> lines=getLines();
  std::vector<AGLine2>::iterator i=lines.begin();
  AGVector2 p;
  //  cdebug(toString());
  for(;i!=lines.end();i++)
    {
      p=i->collisionPoint(l);
      if(p!=invalidVec2)
        ps.push_back(p);
    }
  return ps;
}

AGTriangle2 AGTriangle2::applied(const AGMatrix3 &m) const
{
  return AGTriangle2((m*AGVector3(p[0],1)).dim2(),
      (m*AGVector3(p[1],1)).dim2(),
      (m*AGVector3(p[2],1)).dim2());
}

AGString AGTriangle2::toString() const
{
  std::ostringstream os;
  os<<"["<<p[0].toString()<<" "<<p[1].toString()<<" "<<p[2].toString()<<"]";
  return AGString(os.str());
}

AGVector2 AGTriangle2::get(int index) const
{
  return p[index];
}


AGVector2 AGTriangle2::operator[](int index) const
{
  return p[index];
}

bool AGTriangle2::collide(const AGTriangle2 &t) const
{
  std::vector<AGVector2> v=getNormals();
  append(v,t.getNormals());
  std::vector<AGVector2>::iterator i=v.begin();

  for(;i!=v.end();i++)
    {
      float min1,min2,max1,max2;
      size_t j;
      for(j=0;j<3;j++)
        {
          if(j==0)
            {
              min1=max1=(*i)*p[j];
              min2=max2=(*i)*t.p[j];
            }
          else
            {
              float v0=(*i)*p[j];
              float v1=(*i)*t.p[j];
              min1=std::min(min1,v0);
              max1=std::max(max1,v0);
              min2=std::min(min2,v1);
              max2=std::max(max2,v1);
            }
        }
      if(!collide1d(min1,max1,min2,max2))
        return false;
    }
  return true;
}

std::vector<AGVector2> AGTriangle2::getNormals() const
{
  std::vector<AGVector2> l;
  l.push_back((p[1]-p[0]).normalized().normal());
  l.push_back((p[2]-p[0]).normalized().normal());
  l.push_back((p[2]-p[1]).normalized().normal());
  return l;
}

std::vector<AGLine2> AGTriangle2::getLines() const
{
  std::vector<AGLine2> v;
  v.push_back(AGLine2(p[0],p[1]));
  v.push_back(AGLine2(p[1],p[2]));
  v.push_back(AGLine2(p[2],p[0]));
  return v;
}

bool AGTriangle2::contains(const AGVector2 &pp) const
{
  std::vector<AGVector2> l=getNormals(); // BEWARE: dont' change the order in getNormals!!!


  if(AGsign((pp-p[0])*l[0])==AGsign((p[2]-p[0])*l[0]))
    if(AGsign((pp-p[0])*l[1])==AGsign((p[1]-p[0])*l[1]))
      if(AGsign((pp-p[1])*l[2])==AGsign((p[0]-p[1])*l[2]))
        return true;
  return false;
}


AGVector2 AGTriangle2::touchPoint(const AGTriangle2 &t) const
{
  // assume that one point of a triangle is contained in the other and return this point
  size_t i;
  for(i=0;i<3;i++)
    {
      if(contains(t[i]))
        return t[i];
      else if(t.contains((*this)[i]))
        return (*this)[i];
    }
  return invalidVec2;
}

AGVector2 AGTriangle2::touchVector(const AGTriangle2 &t) const
{
  //#ifdef OLD_TOUCH_APPROXIMATION
  // try to get line, which has two intersections with other triangle's lines

  std::vector<AGLine2> l0=getLines();
  std::vector<AGLine2> l1=t.getLines();

  std::vector<AGLine2>::iterator i,j;

  std::map<AGLine2*,int> sum;

  for(i=l0.begin();i!=l0.end();i++)
    for(j=l1.begin();j!=l1.end();j++)
      {
        if(i->collide(*j))
          {
            sum[&(*i)]++;
            sum[&(*j)]++;
          }
      }
  std::map<AGLine2*,int>::iterator k=sum.begin();
  for(;k!=sum.end();k++)
    {
      if(k->second==2)
        return k->first->normal();
    }

  // FIXME: not found - find other approximation

  //#else
  {
    size_t i;
    for(i=0;i<3;i++)
      {
        if(contains(t[i]))
          return nearestLine(t[i]).normal();
        else if(t.contains((*this)[i]))
          return t.nearestLine((*this)[i]).normal();
      }
    return invalidVec2;
  }
  //#endif
}

AGLine2 AGTriangle2::nearestLine(const AGVector2 &v) const
{
  std::vector<AGLine2> l=getLines();
  std::vector<AGLine2>::iterator i;
  std::map<float,AGLine2> dist;
  for(i=l.begin();i!=l.end();i++)
    {
      dist[i->distance(v)]=*i;
      //      cdebug(i->distance(v)<<i->toString());
    }
  return dist.begin()->second;
}

/////////////////////////////////////////////////////////////////////////////
// AGCircle2
/////////////////////////////////////////////////////////////////////////////

AGCircle2::AGCircle2(const AGVector2 &v,float rad):p(v),radius(rad)
      {
      }

std::list<AGVector2> AGCircle2::collide(const AGLine2 &pLine) const
{
  std::list<AGVector2> l;

  AGVector2 normal=pLine.normal();
  AGVector2 mp(p.dim2());

  float dist=(mp-pLine[0])*normal;

  if(radius>fabs(dist))
    {
      AGVector2 dir=pLine.direction().normalized();
      // ground-point
      float alpha=dir*(mp-pLine[0]);
      AGVector2 g=pLine[0]+dir*alpha;

      float dist=(g-mp).length();
      assert(radius>dist);
      float rest=sqrt(radius*radius-dist*dist);
      l.push_back(g+dir*rest);
      l.push_back(g+dir*(-rest));
    }

  return l;
}


bool AGCircle2::inCircle(const AGVector2 &v) const
{
  return (v-p).length2()<radius*radius;
}
bool AGCircle2::outCircle(const AGVector2 &v) const
{
  return (v-p).length2()>radius*radius;
}

bool AGCircle2::inCircle(const AGTriangle2 &t) const
{
  return inCircle(t.get(0)) && inCircle(t.get(1)) && inCircle(t.get(2));
}
bool AGCircle2::outCircle(const AGTriangle2 &t) const
{
  return outCircle(t.get(0)) && outCircle(t.get(1)) && outCircle(t.get(2));
}

AGVector2 AGCircle2::getPos() const
{
  return p;
}
float AGCircle2::getRadius() const
{
  return radius;
}



/////////////////////////////////////////////////////////////////////////////
// AGTriangle3
/////////////////////////////////////////////////////////////////////////////
AGTriangle3::AGTriangle3(const AGVector3 &v0,const AGVector3 &v1,const AGVector3 &v2)
  {
    p[0]=v0;
    p[1]=v1;
    p[2]=v2;
  }

AGVector4 AGTriangle3::collide(const AGLine3 &pLine) const
{
  AGVector3 normal=(p[1]-p[0])%(p[2]-p[0]);
  AGVector4 mfalse(0,0,0,0);
  if(normal.length2()==0)
    return mfalse; // bad triangle
  normal.normalize();
  float v0=(pLine.getV0()-p[0])*normal;
  float v1=(pLine.getV1()-p[0])*normal;
  float vall=v0-v1;
  float v=v0/vall;

  if(v<0 || v>1)
    return mfalse; // line doesn't intersect triangle's plane

  AGVector3 ip=pLine.getV1()*v+pLine.getV0()*(1-v);

  assert(fabs((ip-p[0])*normal)<0.00001);
  // check if point is on triangle (already checked that point is in plane)
  AGVector3 n0=(p[2]-p[1])%normal;
  AGVector3 n1=(p[2]-p[0])%normal;
  AGVector3 n2=(p[0]-p[1])%normal;
  n0.normalize();
  n1.normalize();
  n2.normalize();

  if(n0==n1 || n0==n2 || n1==n2)
    return mfalse;

  assert(fabs(n0*normal)<0.0001);
  assert(fabs(n1*normal)<0.0001);
  assert(fabs(n2*normal)<0.0001);

  assert(fabs((p[2]-p[1])*n0)<0.0001);
  assert(fabs((p[2]-p[0])*n1)<0.0001);
  assert(fabs((p[0]-p[1])*n2)<0.0001);

  if(AGsign((p[0]-p[1])*n0)!=AGsign((ip-p[1])*n0))
    return mfalse;
  if(AGsign((p[1]-p[2])*n1)!=AGsign((ip-p[2])*n1))
    return mfalse;
  if(AGsign((p[2]-p[1])*n2)!=AGsign((ip-p[1])*n2))
    return mfalse;


  return AGVector4(ip,1);
}

void AGTriangle3::flip()
  {
    AGVector3 tmp=p[2];
    p[2]=p[1];
    p[1]=tmp;
  }

AGString AGTriangle3::toString() const
{
  std::ostringstream os;
  os<<"["<<p[0].toString()<<";"<<p[1].toString()<<";"<<p[2].toString()<<"]";
  return AGString(os.str());
}

AGVector3 AGTriangle3::operator[](int index) const
{
  assert(index>=0);
  assert(index<3);
  return p[index];
}


/////////////////////////////////////////////////////////////////////////////
// AGRect2F
/////////////////////////////////////////////////////////////////////////////

AGRect2::AGRect2(const SDL_Rect &r)
  {
    v0=AGVector2(r.x,r.y);
    v1=AGVector2(r.x+r.w,r.y+r.h);
  }


AGRect2::AGRect2(const AGString &ps)
  {
    std::istringstream is;
    char c;
    float x,y,w,h;
    is.str(ps);
    is>>c;
    is>>x;
    is>>c;
    is>>y;
    is>>c;
    is>>w;
    is>>c;
    is>>h;
    is>>c;
    v0=AGVector2(x,y);
    v1=AGVector2(x+w,y+h);
  }


AGRect2::AGRect2()
  {
  }

AGRect2::AGRect2(const AGVector2 &pv0,const AGVector2 &pv1):
  v0(pv0),v1(pv1)
        {
        }

AGRect2::AGRect2(float x,float y,float w,float h):
  v0(x,y),v1(x+w,y+h)
        {
        }

AGVector2 AGRect2::clip(const AGVector2 &v) const
{
  return AGVector2(std::max(std::min(v[0],v1[0]),v0[0]),
      std::max(std::min(v[1],v1[1]),v0[1])
  );
}


AGRect2 AGRect2::alignGrid() const
{
  return AGRect2((int)x(),
      (int)y(),
      (int)w(),
      (int)h());
}

AGVector2 AGRect2::getMiddle() const
{
  return (v0+v1)/2;
}


void AGRect2::check() const
{
  float mx0=std::min(x0(),x1());
  float my0=std::min(y0(),y1());
  float mx1=std::max(x0(),x1());
  float my1=std::max(y0(),y1());

  AGRect2 *p=const_cast<AGRect2*>(this);
  p->v0[0]=mx0;
  p->v0[1]=my0;
  p->v1[0]=mx1;
  p->v1[1]=my1;
}

AGRect2& AGRect2::operator+=(const AGRect2 &r)
      {
        if(width()==0 || height()==0)
          *this=r;
        else
          {
            include(r[0]);
            include(r[1]);
          }
        return *this;
      }



AGRect2 AGRect2::intersect(const AGRect2 &r) const
{
  check();
  r.check();

  float mx0=std::max(x0(),r.x0());
  float my0=std::max(y0(),r.y0());
  float mx1=std::min(x1(),r.x1());
  float my1=std::min(y1(),r.y1());

  if(mx0>mx1)
    mx0=mx1;
  if(my0>my1)
    my0=my1;

  return AGRect2(AGVector2(mx0,my0),
      AGVector2(mx1,my1));
}

std::vector<AGRect2> AGRect2::difference(const AGRect2 &r) const
{
  std::vector<AGRect2> l;
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      {

        float nx,ny,nw,nh;

        switch(i)
          {
        case 0:
          nx=x0();
          nw=r.x0()-x0();
          break;
        case 1:
          nx=r.x0();
          nw=r.x1()-r.x0();
          break;
        case 2:
          nx=r.x1();
          nw=x1()-r.x1();
          break;
          };
        switch(j)
          {
        case 0:
          ny=y0();
          nh=r.y0()-y0();
          break;
        case 1:
          ny=r.y0();
          nh=r.y1()-r.y0();
          break;
        case 2:
          ny=r.y1();
          nh=y1()-r.y1();
          break;
          };
        if(nw>0 && nh>0)
          {
            AGRect2 n=intersect(AGRect2(nx,ny,nw,nh));
            if(n.w()>0 && n.h()>0)
              if(!r.contains(n))
                l.push_back(n);
          }
      }
  return l;
}



AGVector2 AGRect2::operator[](size_t i) const
{
  switch(i)
    {
  case 0:
    return v0;
  case 1:
    return v1;
  default:
    throw AGString("invalid index in AGRect2::operator[]");
    }
  return v0;
}
AGVector2 &AGRect2::operator[](size_t i)
      {
        switch(i)
          {
        case 0:
          return v0;
        case 1:
          return v1;
        default:
          throw AGString("invalid index in AGRect2::operator[]");
          }
        return v0;
      }

AGRect2 AGRect2::shrink(float f) const
{
  AGVector2 d(f,f);
  return AGRect2(v0+d,v1-d);
}

AGRect2 AGRect2::shrink(float w,float h) const
{
  AGVector2 d(w,h);
  return AGRect2(v0+d,v1-d);
}


AGRect2 AGRect2::shrinkToTopLeft(float w,float h) const
{
  return AGRect2(x(),y(),this->w()-w,this->h()-h);
}


AGRect2 AGRect2::grow(float f) const
{
  AGVector2 d(f,f);
  return AGRect2(v0-d,v1+d);
}


bool AGRect2::operator==(const AGRect2 &r) const
{
  return v0==r.v0 && v1==r.v1;
}
bool AGRect2::operator!=(const AGRect2 &r) const
{
  return v0!=r.v0 || v1!=r.v1;
}

std::vector<AGRect2> AGRect2::tile(size_t x,size_t y) const
{
  std::vector<AGRect2> l;

  if(x==0 || y==0)
  return l;

  float w=width()/x;
  float h=height()/y;

  for(size_t i=0;i<x;i++)
    for(size_t j=0;j<y;j++)
      {
        l.push_back(AGRect2(x0()+w*i,y0()+h*j,w,h));
      }

  return l;
}



SDL_Rect AGRect2::sdl() const
{
  SDL_Rect r;
  r.x=int(v0[0]);
  r.y=int(v0[1]);
  r.w=int(v1[0]-v0[0]+0.1); // prevent shitty rounding error !
  r.h=int(v1[1]-v0[1]+0.1);

  return r;
}

float AGRect2::setWidth(float w)
  {
    v1[0]=v0[0]+w;
    return w;
  }
float AGRect2::setHeight(float h)
  {
    v1[1]=v0[1]+h;
    return h;
  }

float AGRect2::x0() const
{
  return v0[0];
}
float AGRect2::y0() const
{
  return v0[1];
}
float AGRect2::x1() const
{
  return v1[0];
}
float AGRect2::y1() const
{
  return v1[1];
}

AGRect2 AGRect2::origin() const
{
  return AGRect2(0,0,w(),h());
}

float AGRect2::content() const
{
  return (v1[0]-v0[0])*(v1[1]-v0[1]);
}


void AGRect2::include(const AGVector2 &v)
  {
    v0[0]=std::min(v0[0],v[0]);
    v0[1]=std::min(v0[1],v[1]);
    v1[0]=std::max(v1[0],v[0]);
    v1[1]=std::max(v1[1],v[1]);
  }


std::list<AGRect2> AGRect2::split() const
{
  std::list<AGRect2> l;
  float hw=(v1[0]-v0[0])/2;
  float hh=(v1[1]-v0[1])/2;
  l.push_back(AGRect2(v0[0]   ,v0[1]   ,hw,hh));
  l.push_back(AGRect2(v0[0]+hw,v0[1]   ,hw,hh));
  l.push_back(AGRect2(v0[0]   ,v0[1]+hh,hw,hh));
  l.push_back(AGRect2(v0[0]+hw,v0[1]+hh,hw,hh));

  return l;
}


bool AGRect2::contains(const AGRect2 &v) const
{
  return contains(v.getV0()) && contains(v.getV1()) && contains(v.getV01()) && contains(v.getV10());
}
AGVector2 AGRect2::getV0() const
{
  return v0;
}
AGVector2 AGRect2::getV1() const
{
  return v1;
}
AGVector2 AGRect2::getV01() const
{
  return AGVector2(v0[0],v1[1]);
}
AGVector2 AGRect2::getV10() const
{
  return AGVector2(v1[0],v0[1]);
}


bool AGRect2::contains(const AGVector2 &v) const
{
  return (v.getX()>= v0.getX() && v.getY()>=v0.getY() && v.getX()<=v1.getX() && v.getY()<=v1.getY());
}

AGString AGRect2::toString() const
{
  return v0.toString()+":"+v1.toString();
}


void AGRect2::setX(float p)
  {
    float mw=w();
    v0.setX(p);
    v1.setX(p+mw);
  }
void AGRect2::setY(float p)
  {
    float mh=h();
    v0.setY(p);
    v1.setY(p+mh);
  }

void AGRect2::setLeft(float p)
  {
    float mw=w();
    v0.setX(p);
    v1.setX(p+mw);
  }
void AGRect2::setTop(float p)
  {
    float mh=h();
    v0.setY(p);
    v1.setY(p+mh);
  }

void AGRect2::setRight(float p)
  {
    v1.setX(p);
  }
void AGRect2::setBottom(float p)
  {
    v1.setY(p);
  }



bool AGRect2::collide(const AGRect2 &r) const
{
  return collide1d(v0.getX(),v1.getX(),r.v0.getX(),r.v1.getX()) && collide1d(v0.getY(),v1.getY(),r.v0.getY(),r.v1.getY());
}

AGRect2 AGRect2::operator+(const AGVector2 &v) const
{
  return AGRect2(v0+v,v1+v);
}
AGRect2 AGRect2::operator-(const AGVector2 &v) const
{
  return AGRect2(v0-v,v1-v);
}
AGRect2 &AGRect2::operator+=(const AGVector2 &v)
      {
        v0+=v;
        v1+=v;
        return *this;
      }

AGRect2 &AGRect2::operator-=(const AGVector2 &v)
      {
        v0-=v;
        v1-=v;
        return *this;
      }

float AGRect2::x() const
{
  return v0.getX();
}
float AGRect2::y() const
{
  return v0.getY();
}
float AGRect2::w() const
{
  return v1.getX()-v0.getX();
}
float AGRect2::h() const
{
  return v1.getY()-v0.getY();
}

float AGRect2::width() const
{
  return v1.getX()-v0.getX();
}
float AGRect2::height() const
{
  return v1.getY()-v0.getY();
}


/////////////////////////////////////////////////////////////////////////////
// AGLine
/////////////////////////////////////////////////////////////////////////////

AGLine2::AGLine2()
  {
  }

AGLine2::AGLine2(const AGVector2 &pv0,const AGVector2 &pv1):
  v0(pv0),v1(pv1)
        {
        }

AGVector2 &AGLine2::operator[](size_t i)
      {
        switch(i)
          {
        case 0:
          return v0;
        case 1:
          return v1;
        default:
          throw AGString("wrong index in AGLine::op[]");
          }
        return v0;
      }
AGVector2 AGLine2::operator[](size_t i) const
{
  switch(i)
    {
  case 0:
    return v0;
  case 1:
    return v1;
  default:
    throw AGString("wrong index in AGLine::op[]");
    }
  return v0;
}


AGVector2 AGLine2::getV0() const
{
  return v0;
}
AGVector2 AGLine2::getV1() const
{
  return v1;
}

bool AGLine2::has(const AGVector2 &v) const
{
  return v0==v || v1==v;
}

AGVector2 AGLine2::collisionPointNI(const AGLine2 &l) const
{
  float dx, dy;
  float ldx, ldy;
  dx = v1.getX() - v0.getX();
  dy = v1.getY() - v0.getY();
  ldx = l.v1.getX() - l.v0.getX();
  ldy = l.v1.getY() - l.v0.getY();

  AGVector2 p;

  if(dx==0.0f)
    {
      if(ldx==0.0f)
        return invalidVec2; // invalid

      // ldy!=0 !!
      float lm = ldy/ldx;
      float lb = l.v0.getY() - lm * l.v0.getX();

      //      cdebug(lm);
      //      cdebug(lb);

      assert(lm!=0.0f);
      p=AGVector2(v0.getX(),lm*v0.getX()+lb);
    }
  else if(ldx==0.0f)
    {
      p=l.collisionPointNI(*this);
    }
  else
    {
      // ldy!=0 && dy!=0
      float m =dy/dx;
      float lm =ldy/ldx;
      float b = v0.getY() - m * v0.getX();
      float lb=l.v0.getY() - lm * l.v0.getX();

      if(m == lm)
        return AGVector2(0,0); // parallel
      float x=(lb-b)/(m-lm);
      p=AGVector2(x,m*x+b);
    }
  return p;
}
AGVector2 AGLine2::collisionPoint(const AGLine2 &l) const
{
  AGVector2 p=collisionPointNI(l);
  if(p==invalidVec2)
    return p;
  // check for inclusion
  if(includes(p) && l.includes(p))
    {
      assert(distance(p)<0.01);
      assert(l.distance(p)<0.01);
      return p;
    }
  else
    return invalidVec2;
}

bool AGLine2::includes(const AGVector2 &v) const
{
  float f=(v-v0)*((v1-v0).normalized());
  if(f>=0 && f<(v1-v0).length())
    {
      /*    cdebug(v0.toString()<<"  "<<v1.toString()<<"  "<<v.toString());
    cdebug((v-v0).toString()<<"   "<<(v1-v0).toString()<<"   "<<(v1-v0).normalized().toString()<<"   "<<(v1-v0).length());
    cdebug(f);*/
      return true;
    }
  return false;
}

AGRect2 AGLine2::getBBox() const
{
  return AGRect2(std::min(v0.getX(),v1.getX()),std::min(v0.getY(),v1.getY()),fabs(v0.getX()-v1.getX()),fabs(v1.getY()-v0.getY()));
}

bool AGLine2::collide(const AGLine2 &l) const
{
  AGVector2 d0,d1;
  AGVector2 n0,n1;

  d0=direction();
  d1=l.direction();
  n0=normal();
  n1=l.normal();

  bool t0=collide1d(v0*d0,v1*d0,l.v0*d0,l.v1*d0,false);
  bool t1=collide1d(v0*d1,v1*d1,l.v0*d1,l.v1*d1,false);

  bool t2=collide1d(v0*n0,v1*n0,l.v0*n0,l.v1*n0,false);
  bool t3=collide1d(v0*n1,v1*n1,l.v0*n1,l.v1*n1,false);

  return t0 && t1 && t2 && t3;
}

AGVector2 AGLine2::normal() const
{
  return (v1-v0).normalized().normal();
}

AGVector2 AGLine2::direction() const
{
  return v1-v0;
}



AGString AGLine2::toString() const
{
  std::ostringstream os;
  os<<"("<<v0.toString()<<"-"<<v1.toString()<<")";
  return AGString(os.str());
}

float AGLine2::distance(const AGVector2 &v) const
{
  return fabs(normal()*(v-v0));
}


/////////////////////////////////////////////////////////////////////////////
// AGLine3
/////////////////////////////////////////////////////////////////////////////

AGLine3::AGLine3()
  {
  }

AGLine3::AGLine3(const AGVector3 &pv0,const AGVector3 &pv1):
  v0(pv0),v1(pv1)
        {
        }

float AGLine3::distance(const AGVector3 &p) const
{
  AGVector3 dir=direction();
  AGVector3 diff=p-v0;
  AGVector3 normal1=dir%diff;
  AGVector3 plane_normal=dir%normal1;
  plane_normal.normalize();
  return fabs(diff*plane_normal);
}


AGVector3 AGLine3::getV0() const
{
  return v0;
}
AGVector3 AGLine3::getV1() const
{
  return v1;
}

bool AGLine3::has(const AGVector3 &v) const
{
  return v0==v || v1==v;
}

AGVector3 AGLine3::direction() const
{
  return v1-v0;
}


AGString AGLine3::toString() const
{
  AGStringStream os;
  os<<"("<<v0.toString()<<"-"<<v1.toString()<<")";
  return os.str();
}
/*
float AGLine3::distance(const AGVector3 &v) const
{
  return fabs(normal()*(v-v0));
  }*/



///////////////////////////////////////////////////////////////
// AGVector4
///////////////////////////////////////////////////////////////


AGVector4::AGVector4(float pX,float pY,float pZ,float pW)
  {
    v[0]=pX;
    v[1]=pY;
    v[2]=pZ;
    v[3]=pW;
  }
AGVector4::AGVector4(const AGVector4 &a)
  {
    v[0]=a.v[0];
    v[1]=a.v[1];
    v[2]=a.v[2];
    v[3]=a.v[3];
  }
AGVector4::AGVector4(const AGVector3 &a,float h)
  {
    v[0]=a.v[0];
    v[1]=a.v[1];
    v[2]=a.v[2];
    v[3]=h;
  }

AGVector3 AGVector4::dim3() const
{
  return AGVector3(v[0],v[1],v[2]);
}
AGVector2 AGVector4::dim2() const
{
  return AGVector2(v[0],v[1]);
}


AGVector4::AGVector4()
  {
    v[0]=v[1]=v[2]=0.0f;
    v[3]=1.0f;
  }

AGVector4 AGVector4::operator-() const
{
  return AGVector4(-v[0],-v[1],-v[2],-v[3]);
}


void AGVector4::setX(float pX)
  {
    v[0]=pX;
  }
void AGVector4::setY(float pY)
  {
    v[1]=pY;
  }
void AGVector4::setZ(float pZ)
  {
    v[2]=pZ;
  }
void AGVector4::setW(float pW)
  {
    v[3]=pW;
  }

float AGVector4::getX() const
{
  return v[0];
}
float AGVector4::getY() const
{
  return v[1];
}
float AGVector4::getZ() const
{
  return v[2];
}
float AGVector4::getW() const
{
  return v[3];
}

AGVector4 AGVector4::operator-(const AGVector4 &p) const
{
  return AGVector4(v[0]-p.v[0],
      v[1]-p.v[1],
      v[2]-p.v[2],
      v[3]-p.v[3]);
}
AGVector4 AGVector4::operator+(const AGVector4 &p) const
{
  return AGVector4(v[0]+p.v[0],
      v[1]+p.v[1],
      v[2]+p.v[2],
      v[3]+p.v[3]);
}
AGVector4 &AGVector4::operator+=(const AGVector4 &p)
      {
        v[0]+=p.v[0];
        v[1]+=p.v[1];
        v[2]+=p.v[2];
        v[3]+=p.v[3];
        return *this;
      }
AGVector4 &AGVector4::operator-=(const AGVector4 &p)
      {
        v[0]-=p.v[0];
        v[1]-=p.v[1];
        v[2]-=p.v[2];
        v[3]-=p.v[3];
        return *this;
      }

float AGVector4::operator*(const AGVector4 &p) const
{
  return v[0]*p.v[0]+
  v[1]*p.v[1]+
  v[2]*p.v[2]+
  v[3]*p.v[3];
}
AGVector4 &AGVector4::operator*=(float f)
      {
        v[0]*=f;
        v[1]*=f;
        v[2]*=f;
        v[3]*=f;
        return *this;
      }
AGVector4 &AGVector4::operator/=(float f)
      {
        v[0]/=f;
        v[1]/=f;
        v[2]/=f;
        v[3]/=f;
        return *this;
      }
AGVector4 AGVector4::operator*(float f) const
{
  return AGVector4(v[0]*f,v[1]*f,v[2]*f,v[3]*f);
}
AGVector4 AGVector4::operator/(float f) const
{
  return AGVector4(v[0]/f,v[1]/f,v[2]/f,v[3]/f);
}

bool AGVector4::operator==(const AGVector4 &a) const
{
  return v[0]==a.v[0] &&
  v[1]==a.v[1] &&
  v[2]==a.v[2] &&
  v[3]==a.v[3];
}
bool AGVector4::operator!=(const AGVector4 &a) const
{
  return !operator==(a);
}

float AGVector4::length() const
{
  return sqrt(length2());
}
float AGVector4::length2() const
{
  return v[0]*v[0]+
  v[1]*v[1]+
  v[2]*v[2]+
  v[3]*v[3];
}

float AGVector4::length3() const
{
  return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

AGVector4 AGVector4::normalized() const
{
  return *this*(1.0f/length());
}
void AGVector4::normalize()
  {
    operator*=(1.0f/length());
  }

AGVector4 AGVector4::normalized3() const
{
  return *this*(1.0f/length3());
}
void AGVector4::normalize3()
  {
    operator*=(1.0f/length3());
  }

float &AGVector4::operator[](int index)
      {
        assert(index>=0 && index<4);
        return v[index];
      }

float AGVector4::operator[](int index) const
{
  assert(index>=0 && index<4);
  return v[index];
}

bool AGVector4::nonZero() const
{
  return v[0]!=0 ||
  v[1]!=0 ||
  v[2]!=0 ||
  v[3]!=0;
}

AGString AGVector4::toString() const
{
  AGStringStream os;
  os<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<","<<v[3]<<")";
  return os.str();
}

(AGVector4::operator float*)()
      {
        return v;
      }

(AGVector4::operator const float*)() const
{
  return v;
}

AGVector4 AGVector4::operator%(const AGVector4 &a) const
{
  return AGVector4(v[1] * a.v[2] - v[2] * a.v[1],
      v[2] * a.v[0] - v[0] * a.v[2],
      v[0] * a.v[1] - v[1] * a.v[0]);
}

///////////////////////////////////////////////////////////////
// AGMatrix4
///////////////////////////////////////////////////////////////

AGMatrix4::AGMatrix4(float v[16])
  {
    for(size_t i=0;i<16;i++)
      a[i]=v[i];
  }
AGMatrix4::AGMatrix4(float angle,const AGVector3&d)
  {
    float co=cos(angle);
    float si=sin(angle);
    /*
  set(0,0,co + d[0]*d[0]*(1-co));
  set(0,1,d[0]*d[1]*(1-co)-d[2]*si);
  set(0,2,d[0]*d[2]*(1-co)+d[1]*si);

  set(1,0,d[1]*d[0]*(1-co)+d[2]*si);
  set(1,1,co+d[1]*d[1]*(1-co));
  set(1,2,d[1]*d[2]*(1-co)-d[0]*si);

  set(2,0,d[2]*d[0]*(1-co)-d[1]*si);
  set(2,1,d[2]*d[1]*(1-co)+d[0]*si);
  set(2,2,co + d[2]*d[2]*(1-co));
     */
    set(0,0,co + d[0]*d[0]*(1-co));
    set(1,0,d[0]*d[1]*(1-co)-d[2]*si);
    set(2,0,d[0]*d[2]*(1-co)+d[1]*si);

    set(0,1,d[1]*d[0]*(1-co)+d[2]*si);
    set(1,1,co+d[1]*d[1]*(1-co));
    set(2,1,d[1]*d[2]*(1-co)-d[0]*si);

    set(0,2,d[2]*d[0]*(1-co)-d[1]*si);
    set(1,2,d[2]*d[1]*(1-co)+d[0]*si);
    set(2,2,co + d[2]*d[2]*(1-co));

    set(3,0,0);
    set(3,1,0);
    set(3,2,0);

    set(0,3,0);
    set(1,3,0);
    set(2,3,0);

    set(3,3,1);
  }


AGMatrix4::AGMatrix4()
  {
    get(0,1)=get(0,2)=get(0,3)=
      get(1,0)=get(1,2)=get(1,3)=
        get(2,0)=get(2,1)=get(2,3)=
          get(3,0)=get(3,1)=get(3,2)=0.0f;

    get(0,0)=get(1,1)=get(2,2)=get(3,3)=1.0f;
  }

AGMatrix4::AGMatrix4(const AGVector4 &n)
  {
    get(0,0)=get(1,1)=get(2,2)=get(3,3)=1.0f;
    get(0,1)=get(0,2)=get(0,3)=
      get(1,0)=get(1,2)=get(1,3)=
        get(2,0)=get(2,1)=get(2,3)=0.0f;
    get(3,0)=n[0];
    get(3,1)=n[1];
    get(3,2)=n[2];
  }

AGMatrix4 &AGMatrix4::operator+=(const AGMatrix4 &m)
      {
        for(size_t x=0;x<4;x++)
          for(size_t y=0;y<4;y++)
            get(x,y)+=m.get(x,y);
        return *this;
      }

AGMatrix4 AGMatrix4::transposed() const
{
  AGMatrix4 a;
  for(size_t x=0;x<4;x++)
    for(size_t y=0;y<4;y++)
      a.get(x,y)=get(y,x);
  return a;
}



AGMatrix4 AGMatrix4::inverted() const
{
  // gauss-alg.
  AGMatrix4 a;
  AGMatrix4 b(*this);

  gauss(a,b,4);
  return a;
}

void AGMatrix4::swapRows(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<4);
    assert(b<4);
    for(size_t x=0;x<4;x++)
      {
        float t=get(x,a);
        set(x,a,get(x,b));
        set(x,b,t);
      }
  }

void AGMatrix4::swapCols(size_t a,size_t b)
  {
    if(a==b)
      return;
    assert(a<4);
    assert(b<4);
    for(size_t y=0;y<4;y++)
      {
        float t=get(a,y);
        set(a,y,get(b,y));
        set(b,y,t);
      }
  }


AGMatrix3 AGMatrix4::get3x3(size_t x,size_t y) const
{
  AGMatrix3 m;
  assert(x<2);
  assert(y<2);
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      m.set(i,j,get(i+x,j+y));
  return m;
}


void AGMatrix4::set(size_t x,size_t y,float f)
  {
    assert(x>=0 && x<4);
    assert(y>=0 && y<4);
    a[x*4+y]=f;
  }
float AGMatrix4::get(size_t x,size_t y) const
{
  assert(x>=0 && x<4);
  assert(y>=0 && y<4);
  return a[4*x+y];
}

float &AGMatrix4::get(size_t x,size_t y)
  {
    assert(x>=0 && x<4);
    assert(y>=0 && y<4);
    return a[4*x+y];
  }

AGMatrix4 AGMatrix4::operator*(const AGMatrix4 &m) const
{
  AGMatrix4 n;
  size_t x,y;
  for(x=0;x<4;x++)
    for(y=0;y<4;y++)
      n(x,y)=get(0,y)*m(x,0)+
      get(1,y)*m(x,1)+
      get(2,y)*m(x,2)+
      get(3,y)*m(x,3);
  return n;
}
AGMatrix4 &AGMatrix4::operator*=(const AGMatrix4 &m)
      {
        *this=(*this)*m;
        return *this;
      }

AGVector4 AGMatrix4::operator*(const AGVector4 &v) const
{
  return AGVector4(get(0,0)*v[0]+get(1,0)*v[1]+get(2,0)*v[2]+get(3,0)*v[3],
      get(0,1)*v[0]+get(1,1)*v[1]+get(2,1)*v[2]+get(3,1)*v[3],
      get(0,2)*v[0]+get(1,2)*v[1]+get(2,2)*v[2]+get(3,2)*v[3],
      get(0,3)*v[0]+get(1,3)*v[1]+get(2,3)*v[2]+get(3,3)*v[3]);
}

AGString AGMatrix4::toString() const
{
  std::ostringstream os;
  os<<"("<<get(0,0)<<","<<get(1,0)<<","<<get(2,0)<<","<<get(3,0)<<")"<<std::endl;
  os<<"("<<get(0,1)<<","<<get(1,1)<<","<<get(2,1)<<","<<get(3,1)<<")"<<std::endl;
  os<<"("<<get(0,2)<<","<<get(1,2)<<","<<get(2,2)<<","<<get(3,2)<<")"<<std::endl;
  os<<"("<<get(0,3)<<","<<get(1,3)<<","<<get(2,3)<<","<<get(3,3)<<")"<<std::endl;
  return AGString(os.str());
}

(AGMatrix4::operator float*)()
      {
        return a;
      }

(AGMatrix4::operator const float*)() const
{
  return a;
}

/*
double gMatrixDoubleBuf[16];
(AGMatrix4::operator const double*)() const
{
  for(int i=0;i<16;i++)
    gMatrixDoubleBuf[i]=a[i];
  return gMatrixDoubleBuf;
  }*/

float AGMatrix4::operator()(size_t x,size_t y) const
{
  return get(x,y);
}
float &AGMatrix4::operator()(size_t x,size_t y)
      {
        return get(x,y);
      }

AGVector4 AGMatrix4::getRow(size_t i) const
{
  //  return AGVector4(get(i,0),get(i,1),get(i,2),get(i,3));
  return AGVector4(get(0,i),get(1,i),get(2,i),get(3,i));
}


AGMatrix4::MRow AGMatrix4::operator[](size_t y)
      {
        MRow r;
        r.matrix=this;
        r.y=y;
        return r;
      }
/*
const AGMatrix4::Row AGMatrix4::operator[](size_t y) const
{
  Row r;
  r.matrix=const_cast<AGMatrix4*>(this);
  r.y=y;
  return r;
  }*/


///////////////////////////////////////////////////////////////
// AGBox3
///////////////////////////////////////////////////////////////

AGBox3::AGBox3(const AGVector3 &pBase,const AGVector3 &pDir):
  base(pBase),dir(pDir)
        {
          // assert that pDir[i]>0
          for(size_t i=0;i<3;i++)
            if(dir[i]<0)
              {
                base[i]+=dir[i];
                dir[i]=-dir[i];
              }
          mValid=true;
        }



AGBox3::AGBox3():base(AGVector3(0,0,0)),dir(AGVector3(0,0,0))
      {
        mValid=false;
      }

bool AGBox3::valid() const
{
  return mValid;
}


bool AGBox3::collides(const AGMatrix4 &frustum) const
{
  std::vector<AGVector3> a=getVertices();
  float minx,miny,minz;
  float maxx,maxy,maxz;

  minx=miny=minz=10000;
  maxx=maxy=maxz=-10000;
  // simply check, if any of the box's vertices lie inside the frustum
  for(std::vector<AGVector3>::iterator i=a.begin();i!=a.end();i++)
    {
      AGVector4 p(frustum*AGVector4(*i,1));
      p/=p[3];
      if(p[0]>=-1 && p[0]<=1 &&
          p[1]>=-1 && p[1]<=1 &&
          p[2]>=-1 && p[2]<=1)
        return true;

      minx=std::min(minx,p[0]);
      miny=std::min(miny,p[1]);
      minz=std::min(minz,p[2]);

      maxx=std::max(maxx,p[0]);
      maxy=std::max(maxy,p[1]);
      maxz=std::max(maxz,p[2]);

    }

  return AGBox3(AGVector3(-1,-1,-1),AGVector3(2,2,2)).collides(AGBox3(AGVector3(minx,miny,minz),AGVector3(maxx-minx,maxy-miny,maxz-minz)));
}

bool AGBox3::collides(const AGBox3 &box) const
{
  AGVector3 a=base+dir;
  AGVector3 b=box.base+box.dir;

  /*  cdebug(base.toString()<<"   "<<a.toString());
  cdebug(box.base.toString()<<"   "<<b.toString());

  cdebug(collide1d(base[0],a[0],box.base[0],b[0]));
  cdebug(collide1d(base[1],a[1],box.base[1],b[1]));
  cdebug(collide1d(base[2],a[2],box.base[2],b[2]));
   */

  return collide1d(base[0],a[0],box.base[0],b[0]) &&
  collide1d(base[1],a[1],box.base[1],b[1]) &&
  collide1d(base[2],a[2],box.base[2],b[2]);
}



std::vector<AGVector3> AGBox3::getVertices() const
{
  std::vector<AGVector3> a;
  a.push_back(base+AGVector3(0,0,0));
  a.push_back(base+AGVector3(dir[0],0,0));
  a.push_back(base+AGVector3(0,dir[1],0));
  a.push_back(base+AGVector3(0,0,dir[2]));

  a.push_back(base+AGVector3(dir[0],dir[1],0));
  a.push_back(base+AGVector3(dir[0],0,dir[2]));
  a.push_back(base+AGVector3(0,dir[1],dir[2]));
  a.push_back(base+AGVector3(dir[0],dir[1],dir[2]));
  return a;
}

AGBox3 AGBox3::operator-(const AGVector3 &v) const
{
  if(valid())
    return AGBox3(base-v,dir);
  else
    return AGBox3();
}
AGBox3 AGBox3::operator+(const AGVector3 &v) const
{
  if(valid())
    return AGBox3(base+v,dir);
  else
    return AGBox3();
}



void AGBox3::include(const AGVector3&p)
  {
    if(!mValid)
      {
        base=p;
        dir=AGVector3(0,0,0);
        mValid=true;
        return;
      }
    AGVector3 b=base,b2=base+dir;

    base[0]=std::min(b[0],p[0]);
    base[1]=std::min(b[1],p[1]);
    base[2]=std::min(b[2],p[2]);
    AGVector3 n;
    n[0]=std::max(b2[0],p[0]);
    n[1]=std::max(b2[1],p[1]);
    n[2]=std::max(b2[2],p[2]);
    dir[0]=n[0]-base[0];
    dir[1]=n[1]-base[1];
    dir[2]=n[2]-base[2];
  }

bool AGBox3::collides(const AGVector3&p) const
{
  return p[0]>=base[0] && p[1]>=base[1] && p[2]>=base[2] &&
  p[0]<base[0]+dir[0] && p[1]<base[1]+dir[1] && p[2]<base[2]+dir[2];
}
bool AGBox3::collides(const AGLine3&p) const
{
  // FIXME: implement like this:http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=309

  std::vector<AGRect3> sides=getSides();
  for(std::vector<AGRect3>::iterator i=sides.begin();i!=sides.end();++i)
    {
      if((*i).collides(p))
        return true;
    }
  return false;
}

bool AGBox3::contains(const AGTriangle3 &t) const
{
  for(size_t i=0;i<3;i++)
    if(!collides(t[i]))
      return false;
  return true;
}


std::vector<AGRect3> AGBox3::getSides() const
{
  AGVector3 b2=base+dir;
  std::vector<AGRect3> s;
  s.push_back(AGRect3(base,AGVector3(dir[0],dir[1],0)));
  s.push_back(AGRect3(base,AGVector3(dir[0],0,dir[2])));
  s.push_back(AGRect3(base,AGVector3(0,dir[1],dir[2])));
  s.push_back(AGRect3(b2,AGVector3(-dir[0],-dir[1],0)));
  s.push_back(AGRect3(b2,AGVector3(-dir[0],0,-dir[2])));
  s.push_back(AGRect3(b2,AGVector3(0,-dir[1],-dir[2])));
  return s;
}

bool AGBox3::includes(const AGBox3 &b)
  {
    AGVector3 u0=base+dir;
    AGVector3 u1=b.base+b.dir;
    return (b.base[0]>=base[0] && b.base[1]>=base[1] && b.base[2]>=base[2] &&
        u1[0]<=u0[0] && u1[1]<=u0[1] && u1[2]<=u0[2]);
  }

AGString AGBox3::toString() const
{
  std::ostringstream os;
  os<<"["<<base.toString()<<";"<<dir.toString()<<"]";
  return AGString(os.str());
}

std::vector<AGBox3> AGBox3::split() const
{
  std::vector<AGBox3> r;
  AGVector3 d=dir*0.5;
  AGVector3 x(dir[0],0,0);
  AGVector3 y(0,dir[1],0);
  AGVector3 z(0,0,dir[2]);
  r.push_back(AGBox3(base      ,d));
  r.push_back(AGBox3(base+x    ,d));
  r.push_back(AGBox3(base  +y  ,d));
  r.push_back(AGBox3(base+x+y  ,d));
  r.push_back(AGBox3(base    +z,d));
  r.push_back(AGBox3(base+x  +z,d));
  r.push_back(AGBox3(base  +y+z,d));
  r.push_back(AGBox3(base+x+y+z,d));
  return r;
}

///////////////////////////////////////////////////////////////
// AGRect3
///////////////////////////////////////////////////////////////

AGRect3::AGRect3(const AGVector3 &pBase,const AGVector3 &pDir):
  base(pBase),dir(pDir)
        {
          // assert that dir[i]>=0
          for(size_t i=0;i<3;i++)
            if(dir[i]<0)
              {
                base[i]+=dir[i];
                dir[i]=-dir[i];
              }
        }

bool AGRect3::collides(const AGLine3&pLine) const
{
  AGVector3 d0,d1; // directions of rect's sides
  if(dir[0]==0)
    {
      d0=AGVector3(0,dir[1],0);
      d1=AGVector3(0,0,dir[2]);
    }
  else if(dir[1]==0)
    {
      d0=AGVector3(dir[0],0,0);
      d1=AGVector3(0,0,dir[2]);
    }
  else
    {
      d0=AGVector3(dir[0],0,0);
      d1=AGVector3(0,dir[1],0);
    }

  AGVector3 normal=d0%d1;

  float v0=(pLine.getV0()-base)*normal;
  float v1=(pLine.getV1()-base)*normal;
  if(AGsign(v0)==AGsign(v1))
    return false;

  float vall=v1-v0;
  float v=-v0/vall;
  AGVector3 ip=pLine.getV1()*v+pLine.getV0()*(1-v);

  //  cdebug("ip:"<<ip.toString());
  //  cdebug("dist:"<<(ip-base)*normal);

  for(size_t i=0;i<3;i++)
    {
      if(fabs(dir[i])<0.0001)
        continue;
      if(ip[i]<base[i] || ip[i]>base[i]+dir[i])
        {
          return false;
        }
    }
  return true;
}

AGString AGRect3::toString() const
{
  std::ostringstream os;
  os<<"["<<base.toString()<<";"<<dir.toString()<<"]";
  return AGString(os.str());
}




std::ostream &operator<<(std::ostream &o,const AGVector2&v)
  {
    o<<v.toString();
    return o;
  }

std::ostream &operator<<(std::ostream &o,const AGVector3&v)
  {
    o<<v.toString();
    return o;
  }

std::ostream &operator<<(std::ostream &o,const AGVector4&v)
  {
    o<<v.toString();
    return o;
  }

std::ostream &operator<<(std::ostream &o,const AGRect2&v)
  {
    o<<v.toString();
    return o;
  }

std::ostream &operator<<(std::ostream &o,const AGBox3&v)
  {
    o<<"("<<v.base<<" "<<v.dir<<"/"<<v.valid()<<")";
    return o;
  }

std::ostream &operator<<(std::ostream &o,const AGMatrix3&v)
  {
    o<<"(";
    o<<"("<<v.get(0,0)<<";"<<v.get(1,0)<<";"<<v.get(2,0)<<"),";
    o<<"("<<v.get(0,1)<<";"<<v.get(1,1)<<";"<<v.get(2,1)<<"),";
    o<<"("<<v.get(0,2)<<";"<<v.get(1,2)<<";"<<v.get(2,2)<<"),";
    o<<")";
    return o;
  }
std::ostream &operator<<(std::ostream &o,const AGMatrix4&v)
  {
    o<<"(";
    o<<"("<<v.get(0,0)<<";"<<v.get(1,0)<<";"<<v.get(2,0)<<v.get(3,0)<<"),";
    o<<"("<<v.get(0,1)<<";"<<v.get(1,1)<<";"<<v.get(2,1)<<v.get(3,1)<<"),";
    o<<"("<<v.get(0,2)<<";"<<v.get(1,2)<<";"<<v.get(2,2)<<v.get(3,2)<<"),";
    o<<"("<<v.get(0,3)<<";"<<v.get(1,3)<<";"<<v.get(2,3)<<v.get(3,3)<<"),";
    o<<")";
    return o;
  }
