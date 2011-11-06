#include "rk_debug.h"

#include "ag_glwidget.h"
#include "ag_screen.h"
#include <GL/glu.h>

AGGLWidget::AGGLWidget(AGWidget *pParent,const AGRect2 &r):
  AGWidget(pParent,r)
{
}

void AGGLWidget::drawGL()
{

}

void AGGLWidget::drawAll(AGPainter &p)
{
  beginGL();
  drawGL();
  endGL();

  // draw children;
  AGPainter p2(p);
  p2.translate(getRect()[0]);
  p2.clip(getRect().origin());
  //  p2.transform(getRect());

  std::list<AGWidget*>::reverse_iterator i=mChildren.rbegin(); // draw from back to front
  //  AGRect2 r2=r.project(mr);
  for(;i!=mChildren.rend();i++)
    (*i)->drawAll(p2);

}

void AGGLWidget::beginGL()
{
  getScreen().fillRect(getRect(),AGColor(0,0,0)); // draw bg-color

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  setPerspective(45.0,1,100);
  glMatrixMode(GL_MODELVIEW);

  AGRect2 r=getRect();

  glViewport(GLint(r.x()),GLint(getScreen().getHeight()-r.y1()),GLsizei(r.w()),GLsizei(r.h()));
  glDepthMask(true);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);



}

float AGGLWidget::getRatio() const
{
  AGRect2 r=getRect();
  return float(r.w())/float(r.h());
}


void AGGLWidget::endGL()
{
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST); // enable depth test
  glDepthFunc(GL_LEQUAL); // set type depth test
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // GL_NICEST // best perspective correction
  glEnable(GL_BLEND);

  glViewport( 0, 0, getScreen().getWidth(), getScreen().getHeight() );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );

  gluOrtho2D(0,getScreen().getWidth(),0,getScreen().getHeight());

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );


  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glDisable(GL_DEPTH_TEST); // enable depth test

  glDepthMask(false);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//NEAREST);//LINEAR);
  glEnable(GL_COLOR_MATERIAL);
}

void AGGLWidget::setPerspective(float openAngle,float pnear,float pfar)
{
  glMatrixMode(GL_PROJECTION);
  GLdouble r=getRatio();
  gluPerspective(openAngle,r,pnear,pfar);
  glGetFloatv(GL_PROJECTION_MATRIX, pMatrix);
}
