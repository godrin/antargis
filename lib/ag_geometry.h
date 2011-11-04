/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_geometry.h
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

#ifndef AG_TRIANGLE_H
#define AG_TRIANGLE_H

#include <SDL.h>
#include <iostream>
#include <list>
#include <vector>
#include <rk_base.h>
#include <rk_string.h>
#include <stdexcept>

/* There are following classes:

   We do NOT use templates here, because of SWIG-problems.

   - AGVector2 - 2d vector
   - AGVector3 - is homogenous 2d and inhom 3d
   - AGVector4 - hom. 3d
   - AGMatrix3 - 3x3 Matrix
   - AGMatrix4 - 4x4 Matrix

   - AGLine2     - 2d Line (eats AGVector3)
   - AGTriangle2 - 2d triangle (eats AGVector3)

   ATTENTION: the classes may NOT be virtual, as they're possibly used in an array, where then the size is NOT sizeof(v[x]) !!!
 */


class AGRect2;
class AGBox3;
class Node;

/** \brief Simple structure holding an angle in degrees
 */
struct AGEXPORT AGAngle {
  float angle;

  /// @param an angle in degrees
  explicit AGAngle(float a);
};

class GeometryException {
public:

  GeometryException(const std::string &p) : s(p) {
  }
#ifdef SWIG
  %rename(to_s) what() const;
#endif

  std::string what() const {
    return s;
  }

private:
  std::string s;
};

class AGEXPORT AGVector2 {
public:
  float v[2];

  AGVector2(const AGString &s);
  AGVector2(float pX, float pY);
  AGVector2(const AGAngle &a);
  AGVector2(const AGVector2 &a);
  AGVector2();

  void setX(float pX);
  void setY(float pY);

  AGVector2 dim2() const;


#ifdef SWIG
  %rename(x) getX() const;
  %rename(y) getY() const;
#endif
  float getX() const;
  float getY() const;

  AGAngle getAngle() const;


  bool operator<(const AGVector2 &p) const;

  AGVector2 operator-() const;
  AGVector2 operator-(const AGVector2 &p) const;
  AGVector2 operator+(const AGVector2 &p) const;
  float operator*(const AGVector2 &p) const;
  AGVector2 operator*(float f) const;
  AGVector2 operator/(float f) const;

  bool operator==(const AGVector2 &a) const;

#ifndef SWIG
  AGVector2 & operator+=(const AGVector2 &p);
  AGVector2 & operator-=(const AGVector2 &p);

  AGVector2 & operator*=(float f);
  AGVector2 & operator/=(float f);

  bool operator!=(const AGVector2 &a) const;
#endif



  float length() const;
  float length2() const;

  AGVector2 normalized() const;
  void normalize();

  AGVector2 normal() const;

#ifdef SWIG
  %rename(__getitem__) operator[](int index) const;
#endif
  float operator[](int index) const;
#ifndef SWIG
  float &operator[](int index);
#endif

  bool nonZero() const;

  void saveXML(Node &node) const;
  void loadXML(const Node &node);

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;

#ifndef SWIG
  (operator float*) ();
  (operator const float*) () const;
#endif
};

// 2-dim homogenous vector

class AGEXPORT AGVector3 {
public:
  float v[3];

  AGVector3(const AGVector2 &p, float h = 1.0f);
  AGVector3(float pX, float pY, float pZ = 0.0f);
  AGVector3(const AGAngle &a);
  AGVector3(const AGVector3 &a);
  AGVector3();

  void setX(float pX);
  void setY(float pY);
  void setZ(float pZ);


#ifdef SWIG
  %rename(x) getX() const;
  %rename(y) getY() const;
  %rename(z) getZ() const;
#endif
  float getX() const;
  float getY() const;
  float getZ() const;

  AGAngle getAngle() const;

  AGVector3 operator-() const;

  AGVector3 operator-(const AGVector3 &p) const;
  AGVector3 operator+(const AGVector3 &p) const;
#ifdef SWIG
  %rename(add) operator+=(const AGVector3 &p);
  %rename(sub) operator-=(const AGVector3 &p);
#endif
  AGVector3 & operator+=(const AGVector3 &p);
  AGVector3 & operator-=(const AGVector3 &p);

  float operator*(const AGVector3 &p) const;
#ifndef SWIG
  AGVector3 & operator*=(float f);
  AGVector3 & operator/=(float f);
#endif
  AGVector3 operator*(float f) const;
  AGVector3 operator/(float f) const;
  AGVector3 operator%(const AGVector3 &a) const;

  bool operator==(const AGVector3 &a) const;
#ifndef SWIG
  bool operator!=(const AGVector3 &a) const;
#endif
  
  float length() const;
  float length2() const;

  AGVector3 normalized() const;
  void normalize();

  AGVector3 normal() const;

#ifdef SWIG
  %rename(__getitem__) operator[](int index) const;
#endif
  float operator[](int index) const;
#ifndef SWIG
  float &operator[](int index);
#endif

  bool nonZero() const;

  void saveXML(Node &node) const;
  void loadXML(const Node &node);

  AGVector2 dim2() const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;

#ifndef SWIG
  (operator float*) ();
#endif
};

class AGEXPORT AGMatrix3 {
  float a[3][3];
public:

#ifndef SWIG

  struct Row {
    AGMatrix3 *matrix;
    int y;

    float &operator[](size_t x) {
      return matrix->get(x, y);
    }

    float operator[](size_t x) const {
      return matrix->get(x, y);
    }
  };
#endif
  AGMatrix3();
  AGMatrix3(const AGAngle &a); // rotate
  AGMatrix3(const AGVector3 &a); // transpose
  AGMatrix3(float x, float y); // scale
  AGMatrix3(const AGMatrix3 &m);
  void set(size_t x, size_t y, float f);
  float get(size_t x, size_t y) const;
  float &get(size_t x, size_t y);

  AGMatrix3 operator*(const AGMatrix3 &m) const;
#ifndef SWIG
  AGMatrix3 & operator*=(const AGMatrix3 &m);
#endif

  AGVector3 operator*(const AGVector3 &v) const;

  AGMatrix3 operator-() const;

  AGMatrix3 inverted() const throw (GeometryException);
  AGMatrix3 transposed() const;


#ifndef SWIG
  Row operator[](size_t y);
  const Row operator[](size_t y) const;

#endif

  void swapRows(size_t a, size_t b);
  void swapCols(size_t a, size_t b);

  bool operator==(const AGMatrix3&p) const;


#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;
};

class AGEXPORT AGLine2 {
  AGVector2 v0, v1;
public:
  AGLine2();
  AGLine2(const AGVector2 &pv0, const AGVector2 &pv1);

  AGVector2 getV0() const;
  AGVector2 getV1() const;

  bool has(const AGVector2 &v) const;

  bool collide(const AGLine2 &l) const;
  AGVector2 collisionPoint(const AGLine2 &l) const;
  AGVector2 collisionPointNI(const AGLine2 &l) const; // no inclusion test
  bool includes(const AGVector2 &v) const;

  AGRect2 getBBox() const;

  AGVector2 normal() const;
  AGVector2 direction() const;

  float distance(const AGVector2 &v) const;

#ifndef SWIG
  AGVector2 & operator[](size_t i);
#endif
#ifdef SWIG
  %rename(__getitem__) operator[](size_t index) const;
#endif
AGVector2 operator[](size_t i) const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;
};

class AGEXPORT AGLine3 {
  AGVector3 v0, v1;
public:
  AGLine3();
  AGLine3(const AGVector3 &pv0, const AGVector3 &pv1);

  AGVector3 getV0() const;
  AGVector3 getV1() const;

  bool has(const AGVector3 &v) const;
  /*
bool collide(const AGLine3 &l) const;
AGVector3 collisionPoint(const AGLine3 &l) const;
AGVector3 collisionPointNI(const AGLine3 &l) const; // no inclusion test
bool includes(const AGVector3 &v) const;
   */
  //  AGBox3 getBBox() const;

  //  AGVector3 normal() const;
  AGVector3 direction() const;

  float distance(const AGVector3 &p) const;

  //  float distance(const AGVector3 &v) const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;
};

class AGEXPORT AGCollisionData {
};

class AGRect2;

class AGEXPORT AGTriangle2 {
  AGVector2 p[3];
public:
  AGTriangle2();
  AGTriangle2(const AGVector2 &v0, const AGVector2 &v1, const AGVector2 &v2);

#ifdef SWIG
  %rename(__getitem__) operator[](int index) const;
#endif
  AGVector2 operator[](int index) const;

  AGVector2 get(int index) const;

  bool collide(const AGTriangle2 &t) const;

  // FIXME: sweep-based test
  //  AGCollisionData collide(const AGTriangle &t,const AGVector &v0,const AGVector &v1) const;

  std::vector<AGVector2> getNormals() const;

  void apply(const AGMatrix3 &m);
  AGTriangle2 applied(const AGMatrix3 &m) const;

  // estimate touching point (or middle of touching area)
  AGVector2 touchPoint(const AGTriangle2 &t) const;
  AGVector2 touchVector(const AGTriangle2 &t) const;

  std::vector<AGVector2> collisionPoints(const AGLine2 &l) const;

  bool contains(const AGVector2 &p) const;

  AGRect2 getBBox() const;

  AGLine2 nearestLine(const AGVector2 &v) const;

  std::vector<AGLine2> getLines() const;

  void flip();
#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;
};

class AGEXPORT AGCircle2 {
  AGVector2 p;
  float radius;
public:
  AGCircle2(const AGVector2 &v, float rad);

  bool inCircle(const AGTriangle2 &t) const;
  bool outCircle(const AGTriangle2 &t) const;
  bool inCircle(const AGVector2 &v) const;
  bool outCircle(const AGVector2 &v) const;

  AGVector2 getPos() const;
  float getRadius() const;

  std::list<AGVector2> collide(const AGLine2 &pLine) const;
};

class AGVector4;

class AGEXPORT AGTriangle3 {
  AGVector3 p[3];
public:
  AGTriangle3(const AGVector3 &v0, const AGVector3 &v1, const AGVector3 &v2);

  // (x,y,z,0) for no collision
  // (x,y,z,1) for collision in point (x,y,z)
  AGVector4 collide(const AGLine3 &pLine) const;
#ifdef SWIG
  %rename(to_s) toString() const;
#endif

  AGString toString() const;

#ifdef SWIG
  %rename(__getitem__) operator[](int index) const;
#endif
  AGVector3 operator[](int index) const;

  void flip();
};

class AGEXPORT AGRect2 {
  AGVector2 v0, v1;
public:
  AGRect2();
  AGRect2(const AGVector2 &pv0, const AGVector2 &pv1);
  AGRect2(float x, float y, float w, float h);
  AGRect2(const AGString &ps);
  AGRect2(const SDL_Rect &r);

  bool collide(const AGRect2 &r) const;

  void include(const AGVector2 &v);

  AGRect2 operator+(const AGVector2 &v) const;
  AGRect2 operator-(const AGVector2 &v) const;
#ifndef SWIG
  AGRect2 & operator+=(const AGVector2 &v);
  AGRect2 & operator-=(const AGVector2 &v);

  AGRect2 & operator+=(const AGRect2 &r);
#endif

  bool contains(const AGVector2 &v) const;
  bool contains(const AGRect2 &v) const;

  std::list<AGRect2> split() const;

#ifdef SWIG
  %rename(__getitem__) operator[](size_t i) const;
#endif

  AGVector2 operator[](size_t i) const;
#ifndef SWIG
  AGVector2 & operator[](size_t i);
#endif
  AGVector2 getV0() const;
  AGVector2 getV1() const;
  AGVector2 getV01() const;
  AGVector2 getV10() const;

  AGVector2 getMiddle() const;

  AGRect2 shrink(float f) const;
  AGRect2 shrink(float w, float h) const;
  AGRect2 shrinkToTopLeft(float w, float h) const;
  AGRect2 grow(float f) const;

  void setX(float p);
  void setY(float p);

  void setLeft(float p); // attention - modifies right, too
  void setTop(float p); // attention - modifies bottom, too

  void setRight(float p);
  void setBottom(float p);

  float setWidth(float w);
  float setHeight(float w);

  float x() const;
  float y() const;
  float w() const;
  float h() const;

  float width() const;
  float height() const;

  float x0() const;
  float y0() const;
  float x1() const;
  float y1() const;

  AGVector2 clip(const AGVector2 &v) const;

  AGRect2 origin() const;

  AGRect2 intersect(const AGRect2 &r) const;

  //result=this-r
  std::vector<AGRect2> difference(const AGRect2 &r) const;
  std::vector<AGRect2> tile(size_t x, size_t y) const;

  SDL_Rect sdl() const;

  bool operator==(const AGRect2 &r) const;
#ifndef SWIG
  bool operator!=(const AGRect2 &r) const;
#endif

  void check() const;

  AGRect2 alignGrid() const; // align to integer grid

  float content() const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;
};

class AGEXPORT AGVector4 {
public:
  float v[4];

  AGVector4(float pX, float pY, float pZ, float pW = 1.0f);
  AGVector4(const AGVector4 &a);
  AGVector4(const AGVector3 &a, float h);
  AGVector4();

  void setX(float pX);
  void setY(float pY);
  void setZ(float pZ);
  void setW(float pW);


#ifdef SWIG
  %rename(x) getX() const;
  %rename(y) getY() const;
  %rename(z) getZ() const;
  %rename(w) getW() const;
#endif
  float getX() const;
  float getY() const;
  float getZ() const;
  float getW() const;

  AGVector4 operator-(const AGVector4 &p) const;
  AGVector4 operator+(const AGVector4 &p) const;
#ifdef SWIG
  %rename(add) operator+=(const AGVector4 &p);
  %rename(sub) operator-=(const AGVector4 &p);
#endif
  AGVector4 & operator+=(const AGVector4 &p);
  AGVector4 & operator-=(const AGVector4 &p);

  float operator*(const AGVector4 &p) const;
#ifndef SWIG
  AGVector4 & operator*=(float f);
  AGVector4 & operator/=(float f);
#endif
  AGVector4 operator*(float f) const;
  AGVector4 operator/(float f) const;
  AGVector4 operator-() const;

  bool operator==(const AGVector4 &a) const;
  bool operator!=(const AGVector4 &a) const;

  float length() const;
  // length^2
  float length2() const;

  // length 3-dim vector
  float length3() const;


  AGVector4 normalized() const;
  void normalize();

  // 3-dimensional
  AGVector4 normalized3() const;
  void normalize3();

#ifdef SWIG
  %rename(__getitem__) operator[](int index) const;
#endif

  float operator[](int index) const;
#ifndef SWIG
  float &operator[](int index);
#endif
  
  bool nonZero() const;

  // vector-cross-product
  // ignores 4th coordinate
  AGVector4 operator%(const AGVector4 &a) const;

  AGVector4 cross(const AGVector4 &a) const {
    return operator%(a);
  }


  AGVector3 dim3() const;
  AGVector2 dim2() const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;

#ifndef SWIG
  (operator float*) ();
  (operator const float*) () const;
#endif
};

class AGEXPORT AGMatrix4 {
  float a[16];
public:

  struct MRow {
    AGMatrix4 *matrix;
    int y;

    float &operator[](size_t x) {
      return matrix->get(x, y);
    }

    float operator[](size_t x) const {
      return matrix->get(x, y);
    }
  };

  AGMatrix4();
  AGMatrix4(float v[16]);
  AGMatrix4(float angle, const AGVector3 &d);
  AGMatrix4(const AGVector4 &a);
  void set(size_t x, size_t y, float f);
  float get(size_t x, size_t y) const;
  float &get(size_t x, size_t y);

  AGMatrix4 operator*(const AGMatrix4 &m) const;
  AGMatrix4 & operator*=(const AGMatrix4 &m);
  AGMatrix4 & operator+=(const AGMatrix4 &m);

  AGMatrix3 get3x3(size_t x, size_t y) const;

  AGVector4 operator*(const AGVector4 &v) const;

  float operator()(size_t x, size_t y) const;
  float &operator()(size_t x, size_t y);

  AGMatrix4 inverted() const;
  AGMatrix4 transposed() const;

#ifdef SWIG
  %rename(__getitem__) operator[](size_t index) const;
#endif
  MRow operator[](size_t y);
  //  const Row operator[](size_t y) const;

  void swapRows(size_t a, size_t b);
  void swapCols(size_t a, size_t b);

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  AGString toString() const;

#ifndef SWIG
  (operator float*) ();
  (operator const float*) () const;
  //  (operator const double*)() const;
#endif

  AGVector4 getRow(size_t i) const;
};

// axis-aligned

class AGEXPORT AGRect3 {
  AGVector3 base, dir;
public:
  AGRect3(const AGVector3 &pBase, const AGVector3 &pDir);

  bool collides(const AGLine3&pLine) const;

  AGString toString() const;
};

// axis-aligned

class AGEXPORT AGBox3 {
public:
  AGVector3 base, dir;

  AGBox3();
  AGBox3(const AGVector3 &pBase, const AGVector3 &pDir);

  void include(const AGVector3&p);
  bool includes(const AGBox3 &b);
  bool collides(const AGVector3&p) const;

  bool collides(const AGLine3&p) const;
  bool collides(const AGBox3 &b) const;

  bool contains(const AGTriangle3 &t) const;

  std::vector<AGRect3> getSides() const;

  AGString toString() const;

  std::vector<AGBox3> split() const;

  bool collides(const AGMatrix4 &frustum) const;

  std::vector<AGVector3> getVertices() const;
  void calcVertices(std::vector<AGVector3> &vs) const;

  AGBox3 operator+(const AGVector3 &v) const;
  AGBox3 operator-(const AGVector3 &v) const;

  bool valid() const;

private:
  bool mValid;
};

#ifndef SWIG
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGVector2&v);
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGVector3&v);
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGVector4&v);
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGRect2&v);
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGBox3&v);

AGEXPORT std::ostream & operator<<(std::ostream &o, const AGMatrix3&v);
AGEXPORT std::ostream & operator<<(std::ostream &o, const AGMatrix4&v);
#endif

#include "ag_geometry_inline.cc"

#include <ag_serial_vec.h>

#endif
