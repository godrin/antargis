#ifndef AG_GAUSS_H
#define AG_GAUSS_H

#include <math.h>

/// input:a should be a unit-matrix, b contains the matrix to be inverted!
/// afterwards a contains the result
template <class TMatrix> bool gauss(TMatrix &a, TMatrix &b, int size) {
  // lower-left triangle
  for (int c = 0; c < size - 1; c++) // cols
  {
    // FIXME: check that we have a one in (c,c) - otherwise swap rows !
    if (fabs(b.get(c, c)) < 0.0001) {
      int k;
      // find a row where b.get(c,c+n)!=0 for n>1
      for (k = 1; k + c < size; k++)
        if (fabs(b.get(c, c + k)) > 0) {
          // swap rows
          b.swapRows(c, c + k);
          a.swapRows(c, c + k);
          break;
        }
      if (k + c == size)
        return false; // PROBLEM!!!!
    }

    for (int r = c + 1; r < size; r++) // rows
    {
      if (fabs(b.get(c, r)) > 0.0001) {
        float f = -b.get(c, c) / b.get(c, r);
        for (int i = 0; i < size; i++) {
          // modify row
          a.set(i, r, a.get(i, c) + a.get(i, r) * f);
          b.set(i, r, b.get(i, c) + b.get(i, r) * f);
        }
      }
    }
  }
  // upper-right triangle
  for (int c = size - 1; c > 0; c--) // cols
  {
    for (int r = 0; r < c; r++) // rows
    {
      // (c,r) is to be set to zero
      if (fabs(b.get(c, r)) > 0.0001) {
        // search a line >r, that is !=0
        int r2;
        for (r2 = r + 1; r2 < size; r2++)
          if (b.get(c, r2) > 0.0001)
            break;
        if (r2 >= size)
          throw GeometryException("Matrix is not invertable - no cell!=0 found "
                                  "in upper-right triangle.");

        float f = -b.get(c, r2) / b.get(c, r);
        if (f == 0)
          throw GeometryException("Matrix is not invertable - f=0");

        for (int i = 0; i < size; i++) {
          // modify row
          a.set(i, r, a.get(i, r2) + a.get(i, r) * f);
          b.set(i, r, b.get(i, r2) + b.get(i, r) * f);
        }
      }
    }
  }
  // norming
  for (int r = 0; r < size; r++) {
    float v = b.get(r, r);
    if (v != 0)
      for (int c = 0; c < size; c++) {
        a.set(c, r, a.get(c, r) / v);
        b.set(c, r, b.get(c, r) / v);
      }
  }
  return true; // everythin ok
}

#endif
