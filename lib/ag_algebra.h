#ifndef AG_ALGEBRA_H
#define AG_ALGEBRA_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <vector>
#include <rk_string.h>
#include <ag_geometry.h>

class AGEXPORT AGMatrixN
{
 public:
  AGMatrixN(size_t w,size_t h);
  AGMatrixN(const AGMatrixN &p);
  AGMatrixN(const AGMatrix3 &p);
  AGMatrixN(const AGMatrix4 &p);

  void set(size_t x,size_t y,float v);
  float get(size_t x,size_t y) const;

#ifndef SWIG
  AGMatrixN &operator+=(const AGMatrixN &p);
  AGMatrixN &operator*=(const AGMatrixN &p);
#endif
  AGMatrixN operator*(const AGMatrixN &p) const;
  AGMatrixN operator*(float f) const;

  AGMatrixN operator-(const AGMatrixN &p) const;

  float scalar() const;

  AGMatrixN inverse() const;
  AGMatrixN pseudoInverse() const;
  AGMatrixN makeQuadratic() const;

  AGMatrixN transposed() const;

  void makeUnitMatrix();

  void copyFrom(const AGMatrixN &p);

  void output() const;

  AGString toString() const;

  void swapRows(size_t a,size_t b);
  void swapCols(size_t a,size_t b);

  size_t width() const;
  size_t height() const;

  AGMatrixN skipRow(size_t i) const;
  AGMatrixN skipCol(size_t i) const;

 private:

  inline size_t index(size_t x,size_t y) const { return x+y*mW; }

  std::vector<float> m;
  size_t mW,mH;
};

template<class T> float determinant(const T&t);
class AGMatrix3;
class AGMatrix4;

template<> inline float determinant<AGMatrix3>(const AGMatrix3&t)
  {
    return determinant(AGMatrixN(t));
  }
template<> inline float determinant<AGMatrix4>(const AGMatrix4&t)
  {
    return determinant(AGMatrixN(t));
  }

template<class T>
inline bool isInvertable(const T&t)
  {
    return determinant(t)!=0;
  }

#endif
