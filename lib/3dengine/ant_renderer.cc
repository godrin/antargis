#include "ant_renderer.h"
#include "ag_config.h"
#include "ag_fbo.h"
#include "ag_geometry.h"
#include "ag_vdebug.h"
#include "ant_camera.h"
#include "scene.h"

#include <cmath>

bool usePlainGL = true;


#define USE_PERSPECTIVE_SHADOW_MAPS false

Renderer::Renderer() : mCanMultitexture(-1), mCanShadow(-1), mCanGLSL(-1) {
  mFBO = 0;
  mScene = 0;

  AGString shadowType = getConfig()->get("shadowType");
  if (shadowType == "big" || shadowType == "")
    shadowMapSize = 1024;
  else if (shadowType == "small")
    shadowMapSize = 512;
  else
    shadowMapSize = 0;

  switch (shadowMapSize) {
  case 0:
    getConfig()->set("shadowType", "none");
    break;
  case 512:
    getConfig()->set("shadowType", "small");
    break;
  case 1024:
    getConfig()->set("shadowType", "big");
    break;
  }

  shadowInited = false;

  if (!GL_EXT_texture3D)
    std::cerr << "Sorry, Battles of Antargis won't work properly without "
                 "GL_EXT_texture3D extension!"
              << std::endl;
}

Renderer::~Renderer() {
  checkedDelete(mFBO); // checked - no agrubyobject
}

bool Renderer::canMultitexture() {
  if (mCanMultitexture < 0)
    mCanMultitexture = GL_ARB_multitexture;
  return mCanMultitexture;
}

bool Renderer::canShadow() {
  if (mCanShadow < 0)
    mCanShadow = (GL_ARB_shadow && GL_ARB_shadow_ambient);
  return mCanShadow && shadowMapSize > 0;
}

bool Renderer::canGLSL() {
  if (mCanGLSL < 0)
    mCanGLSL = (GL_ARB_vertex_shader && GL_ARB_fragment_shader &&
                GL_ARB_shading_language_100);
  return mCanGLSL;
}

void Renderer::setCurrentScene(Scene *scene) { mScene = scene; }

Scene *Renderer::getCurrentScene() {
  assert(mScene);
  return mScene;
}

GLint Renderer::getShadowUnit() {
  assert(canMultitexture());
  return GL_TEXTURE1;
}
GLint Renderer::getNormalUnit() {
  if (canMultitexture())
    return GL_TEXTURE0;
  else
    return GL_TEXTURE0;
}

void Renderer::initShadowTexture() {
  assertGL;
  if (!canShadow())
    return;

  if (shadowInited)
    return;

  glGenTextures(1, &shadowMapTexture);
  glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
  //  glTexImage2D(   GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize,
  //  shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowMapSize,
               shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  assertGL;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

  shadowInited = true;

  if (canFBO()) {
    mFBO = new AGFBO(shadowMapTexture, shadowMapSize, shadowMapSize); //,true);
  }
  assertGL;
}

void Renderer::beginShadowComputation() {
  assertGL;
  if (!shadowInited)
    initShadowTexture();

  if (mFBO) {
    mFBO->beginDraw();
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assertGL;

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(getCurrentScene()->getLightProj());

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(getCurrentScene()->getLightView());

  // Use viewport the same size as the shadow map
  glViewport(0, 0, shadowMapSize, shadowMapSize);

  // Draw back faces into the shadow map
  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

  // Disable color writes, and use flat shading for speed
  glShadeModel(GL_FLAT);
  glColorMask(0, 0, 0, 0);

  // Draw the scene
  //  Offset the drawing a little back, so that slopy surfaces don't get
  //  shadowed
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1); // was 1,1

  assertGL;
}
void Renderer::endShadowComputation() {
  assertGL;
  glDisable(GL_POLYGON_OFFSET_FILL);
  assertGL;
  // Read the depth buffer into the shadow map texture
  glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
  assertGL;

  /// @todo switch to FBOs - this should bring an improvement of 17 to 19 fps

  if (mFBO) {
    mFBO->endDraw();
  } else {
    glReadBuffer(GL_BACK);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, shadowMapSize,
                     shadowMapSize, 0);

    glReadBuffer(GL_FRONT);
  }

  assertGL;

  // restore states
  assertGL;
  glCullFace(GL_BACK);

  glShadeModel(GL_SMOOTH);
  glColorMask(1, 1, 1, 1);
  assertGL;
}

void Renderer::beginShadowDrawing() {
  assertGL;
  glActiveTexture(getShadowUnit());
  assertGL;

  glMatrixMode(GL_MODELVIEW);
  // draw a flat shadow over
  //  glDisable(GL_LIGHTING);
  //  glEnable(GL_COLOR_MATERIAL);
  assertGL;

  if (usePlainGL) {
    // Calculate texture matrix for projection
    // This matrix takes us from eye space to the light's clip space
    // It is postmultiplied by the inverse of the current view matrix when
    // specifying texgen
    float bias[] = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f}; // bias from [-1,
                                                               // 1] to [0, 1]
    static AGMatrix4 biasMatrix(bias);
    AGMatrix4 textureMatrix = biasMatrix * getCurrentScene()->getLightProj() *
                              getCurrentScene()->getLightView();

    // Set up texture coordinate generation.
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    assertGL;
    glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.getRow(0));
    assertGL;
    glEnable(GL_TEXTURE_GEN_S);
    assertGL;

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    assertGL;
    glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrix.getRow(1));
    assertGL;
    glEnable(GL_TEXTURE_GEN_T);
    assertGL;

    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    assertGL;
    glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrix.getRow(2));
    assertGL;
    glEnable(GL_TEXTURE_GEN_R);

    assertGL;
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    assertGL;
    glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrix.getRow(3));
    assertGL;
    glEnable(GL_TEXTURE_GEN_Q);
    assertGL;
  }

  // Bind & enable shadow map texture
  glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
  glEnable(GL_TEXTURE_2D);

  // Enable shadow comparison
  assertGL;

  if (usePlainGL) {

#ifndef OLD_SHADOW
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,
                    GL_COMPARE_R_TO_TEXTURE);
    assertGL;
    // Shadow comparison should be true (ie not in shadow) if r<=texture
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB,
    //         GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB,
                    GL_LEQUAL); // not needed ???
    assertGL;

    // Shadow comparison should generate an INTENSITY result
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
    assertGL;
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,
                    GL_LUMINANCE); // INTENSITY);
    assertGL;
    //        glTexParameterf(GL_TEXTURE_2D,
    //        GL_TEXTURE_COMPARE_FAIL_VALUE, 1.0f-0.3f);//shadowAlpha);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    assertGL;
    //      glColor4f(0,0,0,0.3);
    assertGL;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#else
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE,
                    GL_INTENSITY); // set this to a "depth texture"
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB,
                    0.5f); // set the compare fail value

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,
                    GL_COMPARE_R_TO_TEXTURE_ARB);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB,
                    GL_LEQUAL); // not needed ???
    assertGL;

    // Shadow comparison should generate an INTENSITY result
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
    assertGL;
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,
                    GL_LUMINANCE); // INTENSITY);

    // Set up the eye plane for projecting the shadow map on the scene

#endif
    // Set alpha test to discard false comparisons
    //   glAlphaFunc(GL_GREATER,0.99f);//LEQUAL, 0.999f);
    //       glAlphaFunc(GL_GEQUAL, 0.99f);
    //       glDisable(GL_ALPHA_TEST);
  }

  glActiveTexture(getNormalUnit());
  assertGL;
}
void Renderer::endShadowDrawing() {
  assertGL;

  glDisable(GL_POLYGON_OFFSET_FILL);

  glActiveTexture(getShadowUnit());
  glBindTexture(GL_TEXTURE_2D, 0);

  if (usePlainGL) {
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
  }
  glActiveTexture(getNormalUnit());
  assertGL;
}

bool Renderer::badShadowMap() { return shadowMapSize == 1024; }

void Renderer::updateMatricesFromCamera(AntCamera &camera) {
  assertGL;

  const AGVector4 &cameraPosition = camera.getCameraPosition();
  const AGVector3 &scenePosition = camera.getPosition();

  // 1. init camera view matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(cameraPosition[0] + scenePosition[0],
            cameraPosition[1] + scenePosition[1],
            cameraPosition[2] + scenePosition[2], scenePosition[0],
            scenePosition[1], scenePosition[2], 0, 0, 1);
  AGMatrix4 modelView, projection;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
  camera.setModelView(modelView);

  // 2. init camera projection matrix

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, ((float)camera.getWidth())/camera.getHeight(), 3.0f, 63.0f);
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  camera.setProjection(projection);
  glMatrixMode(GL_MODELVIEW);
  assertGL;
  if (USE_PERSPECTIVE_SHADOW_MAPS) {
    // PSM
    // calculation of lightposition is somehow crappy

    // PSMs
    //  lightPosition=AGVector4( -2.0, -3, 5.1,1)*100;

    // light View Matrix
    glLoadIdentity();

    AGVector4 lp = camera.getLightPosition();
    //    lp[
    lp[3] = 1;
    lp = projection * modelView * lp;

    lp /= lp[3];

    // it is something like (12,-10,10)

    lp = AGVector4(-0.5, 1.5, -0.5, 1); // should be something like this
    lp *= 100;

    // lp=AGVector4(-2,2,-2,1);
    gluLookAt(lp[0], lp[1], lp[2], 0, 0, 0, 0.0f, 1.0f, 0.0f);

    AGMatrix4 lightView;
    glGetFloatv(GL_MODELVIEW_MATRIX, lightView);

    lightView = lightView * projection * modelView;

    camera.setLightView(lightView);
    // light projection Matrix
    glLoadIdentity();
    //    glOrtho(-10,10,-15,20,10,1000);
    cdebug(lp.toString());
    float s2 = sqrt(2.0f);
    float ldist = lp.length3();

    glOrtho(-s2, s2, -s2, s2, ldist - 2 * s2,
            ldist + 10); // 1,10);//ldist-2*s2,ldist+10*s2);

    // very old:glOrtho(-1,2,-1.5,3,700,750);
    //       glOrtho(-1,2,-1,1,2,8);
AGMatrix4 lightProjection;
    glGetFloatv(GL_MODELVIEW_MATRIX, lightProjection);

    camera.setLightProjection(lightProjection);
  }

  {
    //  lightPosition=AGVector4( -1.0, -3, 5.1,1);

    // calc light view,too
    // light View Matrix
    glLoadIdentity();
    AGVector3 eye = camera.getLightPosition().dim3() + camera.getPosition();
    AGVector3 scenePosition = camera.getPosition();
    AGMatrix4 lightView, lightProjection;
    gluLookAt(eye[0], eye[1], eye[2], 
        scenePosition[0],
              scenePosition[1], scenePosition[2], 0.0f, 0.0f, 1.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightView);
    camera.setLightView(lightView);

    // light projection Matrix
    glLoadIdentity();

    {
      // FIXME add some decent calculation here
      // use getFrustum for estimating a good light-frustum

      float near0 = 20, near1 = 60;
      float far0 = 20, far1 = 110;

      float mnear = sqrt(near0 * near0 + near1 * near1);
      float mfar = sqrt(far0 * far0 + far1 * far1);

      float left = -25;
      float right = 14;
      float bottom = -15;
      float top = 14;

      if (badShadowMap())
        top = bottom + (top - bottom) * 1024.0f / 768.0f;

      glFrustum(left, right, bottom, top, mnear, mfar);
    }

    glGetFloatv(GL_MODELVIEW_MATRIX, lightProjection);
    camera.setLightProjection(lightProjection);
  }

  // viewport
  glMatrixMode(GL_MODELVIEW);
  assertGL;
}
