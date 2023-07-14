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

#include "engine.h"
#include "rk_debug.h"
#include <ag_vdebug.h>

AntCamera::AntCamera(int w, int h) {
  mWidth = w;
  mHeight = h;

  cameraPosition = AGVector4(0, -20, 20);
  lightPosition = AGVector4(-25, -50, 60, 1);
  scenePosition = AGVector4(0, 0, 0, 1);

  cameraPosition = AGVector4(0, -15, 15);
}

void AntCamera::incCameraDistance() {
  float d = cameraPosition[2] + 0.3;

  d = std::min(d, 20.0f);
  cameraPosition = AGVector4(0, -d, d);
}
void AntCamera::decCameraDistance() {
  float d = cameraPosition[2] - 0.3;

  d = std::max(d, 12.0f);
  cameraPosition = AGVector4(0, -d, d);
}


Viewport AntCamera::getViewport() const {
  Viewport p;
  p.viewport[0] = 0;
  p.viewport[1] = 0;
  p.viewport[2] = mWidth;
  p.viewport[3] = mHeight;
  return p;
}
AGMatrix4 AntCamera::getModelview() const { return cameraView; }
AGMatrix4 AntCamera::getProjection() const { return cameraProjection; }

void AntCamera::setPosition(const AGVector3 &p) {
  scenePosition = AGVector4(p, 1);
}

AGVector4 AntCamera::getCameraPosition() const {
  return AGVector4(scenePosition.dim3() + cameraPosition.dim3(), 1);
}
AGVector4 AntCamera::getCameraPositionR() const { return cameraPosition; }

AGVector4 AntCamera::getLightPosition() const {
  return AGVector4(scenePosition.dim3() + lightPosition.dim3(), 1);
}

AGVector3 AntCamera::getPosition() const { return scenePosition.dim3(); }
  
void AntCamera::setModelView(const AGMatrix4 &m) {
  cameraView = m;
}

void AntCamera::setProjection(const AGMatrix4 &m) {
  cameraProjection = m;
}
  
void AntCamera::setLightView(const AGMatrix4 &m) {
  lightView = m;
}

void AntCamera::setLightProjection(const AGMatrix4 &m) {
  lightProjection = m;
}

int AntCamera::getWidth() const { return mWidth; }
int AntCamera::getHeight() const { return mHeight; }

AGMatrix4 AntCamera::getLightComplete() const {
  float bias[] = {
      0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f}; // bias from [-1, 1] to [0, 1]
  static AGMatrix4 biasMatrix(bias);
  return biasMatrix * lightProjection * lightView;
}

AGMatrix4 AntCamera::getLightView() const { return lightView; }
AGMatrix4 AntCamera::getLightProjectionMatrix() const {
  return lightProjection;
}

AntProjection AntCamera::getCameraProjection() const {
  return AntProjection(getModelview(), getProjection(), getViewport());
}
AntProjection AntCamera::getLightProjection() const {
  return AntProjection(getLightView(), getLightProjectionMatrix(),
                       getViewport());
}

void AntCamera::setWidth(int w) {
  mWidth = w;
}
void AntCamera::setHeight(int h) {
  mHeight = h;
}
