/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_rendercontext.cc
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

#include "ag_rendercontext.h"
#include "rk_debug.h"
#include "ag_vdebug.h"
#include "ag_texture.h"
#include "ag_surfacemanager.h"

#define SLOWCONTEXT
//#define TEXTURE_TEST


AGRenderContext gCurrentContext;

AGRenderContext::AGRenderContext():
  mColor(0),
  mTexture(0),
  mProgram(0),
  mLighting(false),
  mDepthWrite(true),
  mDepthTest(true),
  mAlphaThres(-1),
  mAlpha(GL_NONE),
  mCulling(true)
  {
    getSurfaceManager()->registerMe(this);
  }

AGRenderContext::AGRenderContext(const AGRenderContext &c):
  mColor(c.mColor?new AGVector4(*c.mColor):0),
  mTexture(c.mTexture),
  mProgram(c.mProgram),
  mLighting(c.mLighting),
  mDepthWrite(c.mDepthWrite),
  mDepthTest(c.mDepthTest),
  mAlphaThres(c.mAlphaThres),
  mAlpha(c.mAlpha),
  mCulling(c.mCulling)
  {
    getSurfaceManager()->registerMe(this);
  }

AGRenderContext &AGRenderContext::operator=(const AGRenderContext &c)
  {
    mColor=c.mColor?new AGVector4(*c.mColor):0;
    mTexture=c.mTexture;
    mProgram=c.mProgram;
    mLighting=c.mLighting;
    mDepthWrite=c.mDepthWrite;
    mDepthTest=c.mDepthTest;
    mAlphaThres=c.mAlphaThres;
    mAlpha=c.mAlpha;
    mCulling=c.mCulling;


    return *this;
  }


AGRenderContext::~AGRenderContext()
  {
    checkedDelete(mColor);
    getSurfaceManager()->deregisterMe(this);
  }

AGRenderContext *AGRenderContext::getCurrent()
  {
    return &gCurrentContext;
  }


void AGRenderContext::begin()
  {
    assertGL;
    //return;
#ifdef SLOWCONTEXT
    if(mColor)
      {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glColor4fv(*mColor);
      }
    else
      {
        glColor4f(1,1,1,1);
        glDisable(GL_COLOR_MATERIAL);
        //      glColor4f(1,1,1,1);
      }


    glDepthMask(mDepthWrite);

    if(mCulling)
      {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
      }
    else
      glDisable(GL_CULL_FACE);

    if(mTexture)
      {
        if(mTexture->is3d())
          {
            glDisable(GL_TEXTURE_2D);
            glEnable(mTexture->getTarget());
            glBindTexture(mTexture->getTarget(),mTexture->id());
          }
        else
          {
            glDisable(GL_TEXTURE_3D);
            glEnable(mTexture->getTarget());
            glBindTexture(mTexture->getTarget(),mTexture->id());
          }
      }
    else
      {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_3D);
      }

    if(mDepthTest)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);

    if(mLighting)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);


    if(mAlpha!=GL_NONE)
      {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(mAlpha,mAlphaThres);
      }
    else
      glDisable(GL_ALPHA_TEST);




#else

    ///////////////////////////////
    // COLOR

    // color setting

    //  mColor=0;
#warning "This fails in intro somehow!"
    if(mColor!=gCurrentContext.mColor)
      {
        if(mColor)
          {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
          }
        else
          {
            glColor4f(1,1,1,1);
            glDisable(GL_COLOR_MATERIAL);
          }
      }
    if(mColor)
      glColor4fv(*mColor);
    else
      glDisable(GL_COLOR_MATERIAL);

    bool t=false;

    ///////////////////////////////
    // DEPTHWRITE

    if(mDepthWrite!=gCurrentContext.mDepthWrite)
      glDepthMask(mDepthWrite);


    ///////////////////////////////
    // DEPTHTEST
    if(mDepthTest!=gCurrentContext.mDepthTest)
      {
        if(mDepthTest)
          glEnable(GL_DEPTH_TEST);
        else
          glDisable(GL_DEPTH_TEST);
      }



    ///////////////////////////////
    // CULLING

    if(mCulling!=gCurrentContext.mCulling)
      {
        if(mCulling)
          {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
          }
        else
          glDisable(GL_CULL_FACE);
      }
    ///////////////////////////////
    // ALPHA

    //  if(mAlpha!=gCurrentContext.mAlpha)
      {
        if(mAlpha!=GL_NONE)
          glEnable(GL_ALPHA_TEST);
        else
          glDisable(GL_ALPHA_TEST);
      }
      if(mAlpha!=GL_NONE && mAlphaThres!=gCurrentContext.mAlphaThres)
        glAlphaFunc(mAlpha,mAlphaThres);

      ///////////////////////////////
      // TEXTURE

      if(mTexture==gCurrentContext.mTexture)
        {
          // early out
        }
      else
        {
          AGGLTexture *my=mTexture;
          AGGLTexture *last=0;
          if(gCurrentContext.mTexture)
            last=gCurrentContext.mTexture;

          // texture setting
          if(!mTexture)
            {
              if(last)
                {
                  if(last->is3d())
                    {
                      glBindTexture(GL_TEXTURE_3D,0);
                      glDisable(GL_TEXTURE_3D);
                    }
                  else
                    {
                      glBindTexture(GL_TEXTURE_2D,0);
                      glDisable(GL_TEXTURE_2D);
                    }
                  t=true;
                }
            }
          else
            {
              if(!last)
                {
                  // simply enable
                  if(my->is3d())
                    {
                      glEnable(GL_TEXTURE_3D);
                      glBindTexture(GL_TEXTURE_3D,my->id());
                    }
                  else
                    {
                      glEnable(GL_TEXTURE_2D);
                      glBindTexture(GL_TEXTURE_2D,my->id());
                    }
                  t=true;
                }
              else
                {
                  if(my->is3d())
                    {
#ifndef TEXTURE_TEST
                      if(!last->is3d())
                        {
                          glDisable(GL_TEXTURE_2D);
                          glEnable(GL_TEXTURE_3D);
                        }
                      if(last->id()!=my->id())
                        glBindTexture(GL_TEXTURE_3D,my->id());
#else
                      if(!last->is3d())
                        {
                          glDisable(GL_TEXTURE_2D);
                          glEnable(GL_TEXTURE_3D);

                          if(last->id()!=my->id())
                            glBindTexture(GL_TEXTURE_3D,my->id());
                        }
#endif
                    }
                  else
                    {
#ifndef TEXTURE_TEST
                      if(last->is3d())
                        {
                          glDisable(GL_TEXTURE_3D);
                          glEnable(GL_TEXTURE_2D);
                        }
                      if(last->id()!=my->id())
                        {
                          glBindTexture(GL_TEXTURE_2D,my->id());
                        }
#else
                      if(last->is3d())
                        {
                          glDisable(GL_TEXTURE_3D);
                          glEnable(GL_TEXTURE_2D);

                          if(last->id()!=my->id())
                            glBindTexture(GL_TEXTURE_2D,my->id());
                        }
#endif
                    }
                }
            }
        }


      ///////////////////////////////
      // LIGHTING

      // lighting
      if(mLighting!=gCurrentContext.mLighting)
        {
          if(mLighting)
            glEnable(GL_LIGHTING);
          else
            glDisable(GL_LIGHTING);
        }

      gCurrentContext=*this;

#endif
      assertGL;
  }


void AGRenderContext::end()
  {
    // do nothing
  }

void AGRenderContext::setColor(const AGVector4 &pColor)
  {
    checkedDelete(mColor);
    mColor=new AGVector4(pColor);
  }
void AGRenderContext::setColor(const AGColor &pColor)
  {
    checkedDelete(mColor);
    float f=1.0/255.0f;
    mColor=new AGVector4(pColor.r*f,
        pColor.g*f,
        pColor.b*f,
        pColor.a*f);
  }


void AGRenderContext::setTexture(AGGLTexture *pTexture)
  {
    mTexture=pTexture;
  }

void AGRenderContext::setLighting(bool pLight)
  {
    mLighting=pLight;
  }

void AGRenderContext::setDepthWrite(bool w)
  {
    mDepthWrite=w;
  }
void AGRenderContext::setDepthTest(bool w)
  {
    mDepthTest=w;
  }
void AGRenderContext::setCulling(bool c)
  {
    mCulling=c;
  }
void AGRenderContext::setAlpha(float v,GLuint g)
  {
    mAlpha=g;
    mAlphaThres=v;
  }

AGGLTexture *AGRenderContext::getTexture()
  {
    return mTexture;
  }

AGVector4 *AGRenderContext::getColor()
  {
    return mColor;
  }
