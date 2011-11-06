#include "ant_projection.h"

#include <rk_debug.h>

#include <GL/glu.h>


AntProjection::AntProjection()
{
  cdebug("Possible Error: AntProjection not initialized!");
}

AntProjection::AntProjection(const AGMatrix4 &pMv,const AGMatrix4 &pPr,const Viewport &pVp):
  modelview(pMv),projection(pPr),viewport(pVp)
{
}

AGVector3 AntProjection::project(const AGVector3 &p) const
{
  GLdouble x,y,z;

  GLdouble mv[16],pr[16];
  for(int i=0;i<16;i++)
    {
      mv[i]=((const float*)modelview)[i];
      pr[i]=((const float*)projection)[i];
    }

  gluProject(p[0],p[1],p[2],mv,pr,viewport,&x,&y,&z);
  return AGVector3(x,y,z);

}
AGVector3 AntProjection::unProject(const AGVector3 &p) const
{
  GLdouble x,y,z;

  GLdouble mv[16],pr[16];
  for(int i=0;i<16;i++)
    {
      mv[i]=((const float*)modelview)[i];
      pr[i]=((const float*)projection)[i];
    }

  gluUnProject(p[0],p[1],p[2],mv,pr,viewport,&x,&y,&z);
  return AGVector3(x,y,z);
}


AntFrustum AntProjection::getFrustum() const
{
  float w=viewport[2];
  float h=viewport[3];
  float d=1;
  float d0=0;

  AGVector3 p000(unProject(AGVector3(0,0,d0)));
  AGVector3 p100(unProject(AGVector3(w,0,d0)));
  AGVector3 p010(unProject(AGVector3(0,h,d0)));
  AGVector3 p110(unProject(AGVector3(w,h,d0)));
  AGVector3 p001(unProject(AGVector3(0,0,d)));
  AGVector3 p101(unProject(AGVector3(w,0,d)));
  AGVector3 p011(unProject(AGVector3(0,h,d)));
  AGVector3 p111(unProject(AGVector3(w,h,d)));

  std::vector<AntPlane> ps;

  ps.push_back(makePlane(p000,p010,p100)); // front
  ps.push_back(makePlane(p101,p111,p001)); // back
  ps.push_back(makePlane(p010,p011,p110)); // top
  ps.push_back(makePlane(p001,p000,p101)); // bottom
  ps.push_back(makePlane(p001,p011,p000)); // left
  ps.push_back(makePlane(p100,p110,p101)); // right

  return AntFrustum(ps);
}
