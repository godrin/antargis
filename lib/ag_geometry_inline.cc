#ifndef AG_GEOMETRY_INLINE
#define AG_GEOMETRY_INLINE


#include "ag_geometry.h"


inline void AGBox3::calcVertices(std::vector<AGVector3> &vs) const
{
  vs[0]=base+AGVector3(0,0,0);
  vs[1]=base+AGVector3(dir[0],0,0);
  vs[2]=base+AGVector3(0,dir[1],0);
  vs[3]=base+AGVector3(0,0,dir[2]);

  vs[4]=base+AGVector3(dir[0],dir[1],0);
  vs[5]=base+AGVector3(dir[0],0,dir[2]);
  vs[6]=base+AGVector3(0,dir[1],dir[2]);
  vs[7]=base+AGVector3(dir[0],dir[1],dir[2]);
}


inline float AGVector3::operator*(const AGVector3 &p) const
{
  return v[0]*p.v[0]+v[1]*p.v[1]+v[2]*p.v[2];
}


#endif
