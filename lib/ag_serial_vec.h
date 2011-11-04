#ifndef AG_SERIAL_VEC_H
#define AG_SERIAL_VEC_H

#include <ag_serial.h>
#include <ag_geometry.h>

AGEXPORT BinaryIn &operator>>(BinaryIn &i,AGVector2 &v);
AGEXPORT BinaryIn &operator>>(BinaryIn &i,AGVector3 &v);
AGEXPORT BinaryIn &operator>>(BinaryIn &i,AGVector4 &v);

AGEXPORT BinaryOut &operator<<(BinaryOut &i,const AGVector2 &v);
AGEXPORT BinaryOut &operator<<(BinaryOut &i,const AGVector3 &v);
AGEXPORT BinaryOut &operator<<(BinaryOut &i,const AGVector4 &v);


#endif
