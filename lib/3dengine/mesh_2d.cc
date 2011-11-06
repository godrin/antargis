#include "mesh_2d.h"
#include <rk_debug.h>
#include <ag_painter.h>

Mesh2D::Mesh2D(Scene2D *pScene):
  MeshBase(pScene,AGVector4(),AGBox3())
{
}
Mesh2D::Mesh2D(Scene2D *pScene,Mesh2DData &data,const AGVector4 &pPos,float pRot):
  MeshBase(pScene,pPos,data.bbox()),
  mData(&data)
{
}
Mesh2D::~Mesh2D() throw()
{
}

void Mesh2D::draw()
{
  Scene2D *s=dynamic_cast<Scene2D*>(getScene());
  assert(s);
  AGPainter *painter=s->getPainter();

  assert(painter);

  AGRect2 r=getDrawingRect();
  AGTexture *t=mData->getTexture();

  //  cdebug("r:"<<r);
  painter->blit(*t,r);
}

AGRect2 Mesh2D::getDrawingRect()
{
  Scene2D *s=dynamic_cast<Scene2D*>(getScene());
  assert(s);

  AGTexture *t=mData->getTexture();
  AGVector2 middle(s->getPosition(getPos()));
  float w=t->width();
  float h=t->height();
  AGRect2 r(middle.getX()-w/2,middle.getY()-h/2,w,h);

  mLast=r;

  return r;
}

AGRect2 Mesh2D::getLastDrawingRect()
{
  return mLast;
}

AGVector4 Mesh2D::lineHit(const AGLine3 &pLine) const
{
  //FIXME
  return AGVector4();
}

/// set rotation around y-axis (e.g. turn people)
void Mesh2D::setRotation(float r)
{
}
/// set color (for rings)
void Mesh2D::setColor(const AGVector4 &pColor)
{
}
void Mesh2D::setVisible(bool v)
{
}

Mesh2DData *Mesh2D::getData()
{
  return mData;
}

bool Mesh2D::hit(const AGVector2 &screenPos)
{
  AGRect2 dRect=getDrawingRect();
  if(dRect.contains(screenPos))
    {
      // check if texture is !=transparent there
      AGVector2 p=screenPos-dRect.getV0();
      return mData->getTexture()->getPixel(p[0],p[1]).a>10; // some threshold here

    }
  return false;
}
