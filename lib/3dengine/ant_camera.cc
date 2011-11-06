/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ant_camera.cc
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

#include "ant_camera.h"
#include "ant_renderer.h"
#include <SDL_opengl.h>
#include <cmath>

#include "rk_debug.h"

AntCamera::AntCamera(int w,int h)
  {
    mWidth=w;
    mHeight=h;

    cameraPosition=AGVector4(0,-20,20);
    lightPosition=AGVector4( -25, -50, 60,1);
    scenePosition=AGVector4(0,0,0,1);

    cameraPosition=AGVector4(0,-15,15);

    mPSM=false;
    updateMatrices();
  }

void AntCamera::incCameraDistance()
  {
    float d=cameraPosition[2]+0.3;

    d=std::min(d,20.0f);
    cameraPosition=AGVector4(0,-d,d);
    updateMatrices();
  }
void AntCamera::decCameraDistance()
  {
    float d=cameraPosition[2]-0.3;

    d=std::max(d,12.0f);
    cameraPosition=AGVector4(0,-d,d);
    updateMatrices();
  }


void AntCamera::updateMatrices()
  {

    // 1. init camera view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition[0]+scenePosition[0],cameraPosition[1]+scenePosition[1],cameraPosition[2]+scenePosition[2],
        scenePosition[0],scenePosition[1],scenePosition[2],
        0,0,1);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraView);

    // 2. init camera projection matrix

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ((float)mWidth)/mHeight, 3.0f, 63.0f);
    glGetFloatv(GL_PROJECTION_MATRIX, cameraProjection);
    glMatrixMode(GL_MODELVIEW);
    if(mPSM)
      {
        // PSM
        // calculation of lightposition is somehow crappy


        // PSMs
        //  lightPosition=AGVector4( -2.0, -3, 5.1,1)*100;

        // light View Matrix
        glLoadIdentity();

        AGVector4 lp=lightPosition;
        //    lp[
        lp[3]=1;
        lp=cameraProjection*cameraView*lp;

        lp/=lp[3];

        // it is something like (12,-10,10)




        lp=AGVector4(-0.5,1.5,-0.5,1); // should be something like this
        lp*=100;


        //lp=AGVector4(-2,2,-2,1);
        gluLookAt(lp[0], lp[1], lp[2],
            0,0,0,
            0.0f, 1.0f, 0.0f);
        glGetFloatv(GL_MODELVIEW_MATRIX, lightView);

        lightView=lightView*cameraProjection*cameraView;
        // light projection Matrix
        glLoadIdentity();
        //    glOrtho(-10,10,-15,20,10,1000);
        cdebug(lp.toString());
        float s2=sqrt(2.0f);
        float ldist=lp.length3();

        glOrtho(-s2,s2,-s2,s2,ldist-2*s2,ldist+10);//1,10);//ldist-2*s2,ldist+10*s2);


        //very old:glOrtho(-1,2,-1.5,3,700,750);
        //      glOrtho(-1,2,-1,1,2,8);

        glGetFloatv(GL_MODELVIEW_MATRIX, lightProjection);
      }

      {
        //  lightPosition=AGVector4( -1.0, -3, 5.1,1);

        // calc light view,too
        // light View Matrix
        glLoadIdentity();
        gluLookAt(lightPosition[0]+scenePosition[0], lightPosition[1]+scenePosition[1], lightPosition[2]+scenePosition[2],
            scenePosition[0],scenePosition[1],scenePosition[2],
            0.0f, 0.0f, 1.0f);
        glGetFloatv(GL_MODELVIEW_MATRIX, lightView);


        // light projection Matrix
        glLoadIdentity();

          {
#warning "add some decent calculation here"
            // use getFrustum for estimating a good light-frustum

            float near0=20,near1=60;
            float far0=20,far1=110;

            float mnear=sqrt(near0*near0+near1*near1);
            float mfar=sqrt(far0*far0+far1*far1);

            float left=-25;
            float right=14;
            float bottom=-15;
            float top=14;

            if(getRenderer()->badShadowMap())
              top=bottom+(top-bottom)*1024.0f/768.0f;

            glFrustum(left, right, bottom, top,
                mnear,mfar);

          }

          glGetFloatv(GL_MODELVIEW_MATRIX, lightProjection);
      }

      // viewport
      glMatrixMode(GL_MODELVIEW);

  }


Viewport AntCamera::getViewport() const
{
  Viewport p;
  p.viewport[0]=0;
  p.viewport[1]=0;
  p.viewport[2]=mWidth;
  p.viewport[3]=mHeight;
  return p;
}
AGMatrix4 AntCamera::getModelview() const
{
  return cameraView;
}
AGMatrix4 AntCamera::getProjection() const
{
  return cameraProjection;
}

void AntCamera::setPosition(const AGVector3 &p)
  {
    scenePosition=AGVector4(p,1);
    updateMatrices();
  }

AGVector4 AntCamera::getCameraPosition() const
{
  return AGVector4(scenePosition.dim3()+cameraPosition.dim3(),1);
}
AGVector4 AntCamera::getCameraPositionR() const
{
  return cameraPosition;
}

AGVector4 AntCamera::getLightPosition() const
{
  return AGVector4(scenePosition.dim3()+lightPosition.dim3(),1);
}


AGVector3 AntCamera::getPosition() const
{
  return scenePosition.dim3();
}

int AntCamera::getWidth() const
{
  return mWidth;
}
int AntCamera::getHeight() const
{
  return mHeight;
}

AGMatrix4 AntCamera::getLightComplete() const
{
  float bias[]={0.5f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f};        //bias from [-1, 1] to [0, 1]
  static AGMatrix4 biasMatrix(bias);
  return biasMatrix*lightProjection*lightView;
}

AGMatrix4 AntCamera::getLightView() const
{
  return lightView;
}
AGMatrix4 AntCamera::getLightProjectionMatrix() const
{
  return lightProjection;
}

AntProjection AntCamera::getCameraProjection() const
{
  return AntProjection(getModelview(),getProjection(),getViewport());
}
AntProjection AntCamera::getLightProjection() const
{
  return AntProjection(getLightView(),getLightProjectionMatrix(),getViewport());
}

void AntCamera::setWidth(int w)
  {
    mWidth=w;
    updateMatrices();
  }
void AntCamera::setHeight(int h)
  {
    mHeight=h;
    updateMatrices();
  }
