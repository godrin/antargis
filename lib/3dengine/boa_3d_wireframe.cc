#include "boa_3d_wireframe.h"
#include <ag_rendercontext.h>
#include <rk_debug.h>

Boa3dWireframe::Boa3dWireframe(Scene *pScene,const AGVector4 &pColor,float pWidth):
  SceneNode(pScene,AGVector4(0,0,0,0),AGBox3(AGVector3(0,0,0),AGVector3(100,100,100))),
  mWidth(pWidth)
{
  mColor=pColor;
  mChanged=true;
}

Boa3dWireframe::~Boa3dWireframe() throw()
  {}

void Boa3dWireframe::draw()
{
  //  std::cout<<"Mcdebug("<<std::endl;
  //  CTRACE;
  // FIXME: use vertex array ???
  if(mChanged)
    {
      // update vertex array
    }


  AGRenderContext c;
  glLineWidth(mWidth);

  //  c.setDepthTest(false);

  AGVector3 a(0,0,0);
  AGVector3 b(0,0,10);

  c.setColor(mColor);
  c.begin();
  glBegin(GL_LINES);

  for(std::vector<Line>::iterator i=mLines.begin();i!=mLines.end();i++)
    {
      glVertex3fv((float*)a);
      glVertex3fv((float*)b);
      glVertex3fv((float*)i->a);
      glVertex3fv((float*)i->b);
    }
  glEnd();




}

void Boa3dWireframe::addLine(const AGVector3 &a,const AGVector3 &b)
{
  Line line;
  line.a=a;
  line.b=b;
  mLines.push_back(line);
  mChanged=true;

  AGBox3 bb=bbox();

  bb.include(a);
  bb.include(b);
  setBBox(bb);
}

bool Boa3dWireframe::transparent()
{
  return true;
}
