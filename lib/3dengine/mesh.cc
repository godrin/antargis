#include "mesh.h"

#include <map>
#include <vector>
#include <cmath>
#include <ag_vdebug.h>
#include <ag_texturecache.h>
#include <ag_profiler.h>




//////////////////////////////////////////////////////////////////////
// Mesh
//////////////////////////////////////////////////////////////////////

Mesh::Mesh(Scene *pScene):
  MeshBase(pScene,AGVector4(),AGBox3())
  {
    mData=0;
    mRotation=0;
    setOrder(MESH_Z);
    mColor=AGVector4(1,1,1,1);
    mVisible=true;
  }

Mesh::Mesh(Scene *pScene,MeshData &data,const AGVector4 &pPos,float pRot):
  MeshBase(pScene,pPos,data.bbox())
  {
    mData=&data;
    mRotation=pRot;
    setOrder(MESH_Z);
    mColor=AGVector4(1,1,1,1);
    mVisible=true;
  }

Mesh::~Mesh() throw()
  {
  }

void Mesh::draw()
  {
    STACKTRACE;
    if(!mVisible)
      return;

    begin();
    if(mData)
      {
        mData->draw(mColor);
      }
    end();
  }
void Mesh::drawDepth()
  {
    STACKTRACE;
    if(!mVisible)
      return;
    begin();
    if(mData)
      mData->drawDepth();
    end();
  }
void Mesh::drawShadow()
  {
    STACKTRACE;
    if(!mVisible)
      return;
    begin();
    if(mData)
      mData->drawShadow();
    end();
  }

size_t Mesh::getTriangles() const
{
  if(!mData)
    return 0;
  return mData->getTriangles();
}

void Mesh::begin()
  {
    assertGL;

    glPushMatrix();
    AGVector4 p=getPos();
    glTranslatef(p[0],p[1],p[2]);
    glRotatef(mRotation,0.0,0.0,1.0);
		assertGL;
  }
void Mesh::end()
  {
		assertGL;
    glPopMatrix();
		assertGL;
  }


AGVector4 Mesh::lineHit(const AGLine3 &pLine) const
{
  if(!mData)
    return AGVector4(0,0,0,0);
  AGMatrix4 rot(-mRotation*M_PI/180.0,AGVector3(0,0,1));
  AGMatrix4 tr(-getPos());
  AGMatrix4 complete=rot*tr;

  AGVector4 p0(pLine.getV0(),1);
  AGVector4 p1(pLine.getV1(),1);

  p0=complete*p0;
  p1=complete*p1;
  return mData->lineHit(AGLine3(AGVector3(p0[0],p0[1],p0[2]),
      AGVector3(p1[0],p1[1],p1[2])))+AGVector4(getPos().dim3(),0);
}

void Mesh::setRotation(float r)
  {
    mRotation=r;
  }

MeshData *Mesh::getData()
  {
    return mData;
  }

void Mesh::drawPick()
  {
    STACKTRACE;

    if(!mVisible)
      return;
    begin();

    if(mData)
      mData->drawPick();
    end();
  }


bool Mesh::transparent()
  {
    assert(mData);
    return mData->transparent();
  }


void Mesh::setColor(const AGVector4 &pColor)
  {
    mColor=pColor;
  }
void Mesh::setVisible(bool v)
  {
    mVisible=v;
  }

