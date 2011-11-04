#include <ag_serial_vec.h>

BinaryIn &operator>>(BinaryIn &i,AGVector2 &v)
  {
    i>>v[0]>>v[1];
    return i;
  }
BinaryIn &operator>>(BinaryIn &i,AGVector3 &v)
  {
    i>>v[0]>>v[1]>>v[2];
    return i;
  }
BinaryIn &operator>>(BinaryIn &i,AGVector4 &v)
  {
    i>>v[0]>>v[1]>>v[2]>>v[3];
    return i;
  }


BinaryOut &operator<<(BinaryOut &o,const AGVector2 &v)
  {
    o<<v[0]<<v[1];
    return o;
  }

BinaryOut &operator<<(BinaryOut &o,const AGVector3 &v)
  {
    o<<v[0]<<v[1]<<v[2];
    return o;
  }
BinaryOut &operator<<(BinaryOut &o,const AGVector4 &v)
  {
    o<<v[0]<<v[1]<<v[2]<<v[3];
    return o;
  }
