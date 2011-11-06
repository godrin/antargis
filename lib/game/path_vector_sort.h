#ifndef PATH_VECTOR_SORT_H
#define PATH_VECTOR_SORT_H

#include <ag_geometry.h>

class DistanceOrder
{
public:
  DistanceOrder(const AGVector2 &pBase):mBase(pBase)
  {
  }
  bool operator()(const AGVector2 &a,const AGVector2 &b) const
  {
    float la=(a-mBase).length2();
    float lb=(b-mBase).length2();
    if(la<lb)
      return true;
    if(la>lb)
      return false;

    return a<b;
  }
private:
  AGVector2 mBase;

};

#endif
