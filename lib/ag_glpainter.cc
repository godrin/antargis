/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_glpainter.cc
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#include "GLee.h"
#include "ag_glpainter.h"
#include "ag_color.h"
#include "ag_profiler.h"
#include "ag_texture.h"
#include "ag_rendercontext.h"
#include "rk_debug.h"
#include "ag_vdebug.h"
#include "ag_projection.h"

namespace AGGLPainter
  {
    void glColor(const AGColor &c)
      {
        glColor4f(c.r/255.0,c.g/255.0,c.b/255.0,c.a/255.0);
      }

    void fillPoly(const std::vector<AGVector2> &pVecs,const AGColor &pColor)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setColor(pColor);
        context.setCulling(false);
        context.setAlpha(0,GL_NONE);
        context.setDepthTest(false);
        context.begin();

        // turned
        glBegin(GL_POLYGON);
        for(std::vector<AGVector2>::const_iterator i=pVecs.begin();i!=pVecs.end();i++)
          {
            glVertex2fv(*i);
          }
        glEnd();

      }

    void drawPoly(const std::vector<AGVector2> &pVecs,const AGColor &pColor,float pWidth)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setColor(pColor);
        context.setCulling(false);
        context.setAlpha(0,GL_NONE);
        context.setDepthTest(false);
        context.begin();
        glLineWidth(pWidth);

        // turned
        glBegin(GL_LINE_LOOP);
        for(std::vector<AGVector2>::const_iterator i=pVecs.begin();i!=pVecs.end();i++)
          {
            glVertex2fv(*i);
          }
        glEnd();

      }

    void blit(const AGTexture &pSource,const AGRect2 &pRect,const AGRect2 &pSrc,const AGColor &pColor)
      {
        STACKTRACE;

        float tw=pSource.getTextureWidth();
        float th=pSource.getTextureHeight();

        AGRect2 s(pSrc.x()/tw,pSrc.y()/th,pSrc.w()/tw,pSrc.h()/th);


        AGRenderContext context;
        context.setCulling(false);
        context.setTexture(const_cast<AGTexture&>(pSource).glTexture());
        context.setDepthTest(false);
        if(pColor.a>0)
          context.setColor(pColor);
        context.begin();

        glBegin(GL_QUADS);
        glTexCoord2fv(s.getV0());
        glVertex2fv(pRect.getV0());

        glTexCoord2fv(s.getV10());
        glVertex2fv(pRect.getV10());

        glTexCoord2fv(s.getV1());
        glVertex2fv(pRect.getV1());

        glTexCoord2fv(s.getV01());
        glVertex2fv(pRect.getV01());
        glEnd();

      }


    void blit(const AGTexture &pSource,const std::vector<std::pair<AGRect2,AGRect2> > &pSrcDestRects,const AGColor &pColor)
      {
        STACKTRACE;

        float tw=pSource.getTextureWidth();
        float th=pSource.getTextureHeight();

        float itw=1.0/tw;
        float ith=1.0/th;

        AGRenderContext context;
        context.setCulling(false);
        context.setTexture(const_cast<AGTexture&>(pSource).glTexture());
        context.setDepthTest(false);
        if(pColor.a>0)
          context.setColor(pColor);
        context.begin();

        glBegin(GL_QUADS);
        for(std::vector<std::pair<AGRect2,AGRect2> >::const_iterator i=pSrcDestRects.begin();i!=pSrcDestRects.end();i++)
          {
            AGRect2 s(i->first.x()*itw,i->first.y()*ith,i->first.w()*itw,i->first.h()*ith);
            AGRect2 d(i->second);

            glTexCoord2fv(s.getV0());
            glVertex2fv(d.getV0());

            glTexCoord2fv(s.getV10());
            glVertex2fv(d.getV10());

            glTexCoord2fv(s.getV1());
            glVertex2fv(d.getV1());

            glTexCoord2fv(s.getV01());
            glVertex2fv(d.getV01());
          }
        glEnd();
      }


    void tile(const AGTexture &pSource,const AGRect2 &pTarget,const AGColor &pColor)
      {
        STACKTRACE;
        AGRenderContext context;
        context.setCulling(false);
        context.setTexture(const_cast<AGTexture&>(pSource).glTexture());
        context.setDepthTest(false);
        if(pColor.a>0)
          context.setColor(pColor);
        context.begin();

        AGRect2 s(0,0,float(pTarget.width())/pSource.width(),float(pTarget.height())/pSource.height());

        glBegin(GL_QUADS);
        glTexCoord2fv(s.getV0());
        glVertex2fv(pTarget.getV0());

        glTexCoord2fv(s.getV10());
        glVertex2fv(pTarget.getV10());

        glTexCoord2fv(s.getV1());
        glVertex2fv(pTarget.getV1());

        glTexCoord2fv(s.getV01());
        glVertex2fv(pTarget.getV01());


        glEnd();
      }


    void fillRect(const AGRect2 &pRect,const AGColor &c)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setColor(c);
        context.setCulling(false);
        context.setAlpha(0,GL_NONE);
        context.setDepthTest(false);
        context.begin();

        // turned
        glBegin(GL_QUADS);

        glVertex2fv(pRect.getV0());
        glVertex2fv(pRect.getV10());
        glVertex2fv(pRect.getV1());
        glVertex2fv(pRect.getV01());
        glEnd();
      }

    void fillRects(const std::vector<std::pair<AGRect2,AGVector4> > &pRects)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setColor(AGColor(0,0,0));
        context.setCulling(false);
        context.setAlpha(0,GL_NONE);
        context.setDepthTest(false);
        context.begin();

        //    float x0,y0,x1,y1;
        // turned
        glBegin(GL_QUADS);
        for(std::vector<std::pair<AGRect2,AGVector4> >::const_iterator i=pRects.begin();i!=pRects.end();++i)
          {
            glColor4fv(i->second);
            glVertex2fv(i->first.getV0());
            glColor4fv(i->second);
            glVertex2fv(i->first.getV10());
            glColor4fv(i->second);
            glVertex2fv(i->first.getV1());
            glColor4fv(i->second);
            glVertex2fv(i->first.getV01());
          }
        glEnd();
      }


    void drawGradientAlpha(const AGRect2& pRect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setColor(AGVector4(1,1,1,1));
        context.setCulling(false);
        context.setDepthTest(false);


        context.begin();

        glBegin(GL_QUADS);
        glColor(ul);
        glVertex2fv(pRect.getV0());

        glColor(ur);
        glVertex2fv(pRect.getV10());

        glColor(dr);
        glVertex2fv(pRect.getV1());

        glColor(dl);
        glVertex2fv(pRect.getV01());
        glEnd();
      }

    void drawBorder(const AGRect2& rect,int W, const AGColor& c1, const AGColor& c2)
      {
        STACKTRACE;

        AGRect2 inner=rect.shrink(3);

        AGRenderContext context;
        context.setColor(c1);
        context.setCulling(false);
        context.setDepthTest(false);
        context.begin();

        glBegin(GL_QUADS);
        glVertex2fv(rect.getV0());
        glVertex2fv(rect.getV10());
        glVertex2fv(inner.getV10());
        glVertex2fv(inner.getV0());

        glVertex2fv(rect.getV0());
        glVertex2fv(inner.getV0());
        glVertex2fv(inner.getV01());
        glVertex2fv(rect.getV01());

        glEnd();

        // right and //bottom
        context.setColor(c2);
        context.begin();
        //  glColor(c2);
        glBegin(GL_QUADS);

        glVertex2fv(rect.getV10());
        glVertex2fv(rect.getV1());
        glVertex2fv(inner.getV1());
        glVertex2fv(inner.getV10());

        glVertex2fv(rect.getV01());
        glVertex2fv(inner.getV01());
        glVertex2fv(inner.getV1());
        glVertex2fv(rect.getV1());
        glEnd();
      }

    void putPixel(int x,int y,const AGColor &pc)
      {
        STACKTRACE;

        AGRenderContext rc;
        rc.begin();
        float f=0xFF;
        rc.setColor(AGVector4(pc.r/f,pc.g/f,pc.b/f,pc.a/f));
        rc.begin();

        glBegin(GL_POINTS);

        // #warning "really +1 ????????????"
        glVertex2f(x,y+1);

        glEnd();

      }

    void drawLine(const AGVector2 &p0,const AGVector2 &p1,const AGColor &c,float pWidth)
      {
        STACKTRACE;
        assertGL;
        AGRenderContext context;
        context.setColor(c);
        context.setDepthTest(false);
        context.begin();

        glLineWidth(pWidth);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINES);
        glVertex2fv(p0);
        glVertex2fv(p1);

        glEnd();
        assertGL;

      }

    void blitTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle2 &pDest)
      {
        throw int();
        STACKTRACE;
        /*    AGRenderContext c;
    c.setTexture(const_cast<AGTexture&>(pSource).glTexture());
    c.setDepthTest(false);
    c.begin();

    glBegin(GL_TRIANGLES);

    glTexCoord2f(pSrc[2].getX()*pSource.getTW(),1-pSrc[2].getY()*pSource.getTH());
    glVertex2f(pDest[2].getX(), h-1-pDest[2].getY());

    glTexCoord2f(pSrc[1].getX()*pSource.getTW(),1-pSrc[1].getY()*pSource.getTH());
    glVertex2f(pDest[1].getX(), h-1-pDest[1].getY());

    glTexCoord2f(pSrc[0].getX()*pSource.getTW(),1-pSrc[0].getY()*pSource.getTH());
    glVertex2f(pDest[0].getX(), h-1-pDest[0].getY());

    glEnd();*/
      }


    void blit3dTri(const AGTexture &pSource,const AGTriangle2 &pSrc,const AGTriangle3 &pDest)
      {
        STACKTRACE;

        AGRenderContext context;
        context.setCulling(false);
        context.setTexture(const_cast<AGTexture&>(pSource).glTexture());
        context.setDepthTest(true);
        context.setColor(AGColor(0xFF,0xFF,0xFF,0xFF));
        context.begin();

        AGProjection2D p(const_cast<AGTexture&>(pSource).glTexture()->getRect(),AGRect2(0,0,1,1));

        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<3;i++)
          {
            glTexCoord2fv(p.project(pSrc.get(i)));
            //glVertex2f(pDest[i][0],pDest[i][1]);
            glVertex3fv(pDest[i]);
          }

        glEnd();
      }


  }
