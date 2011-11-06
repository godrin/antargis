#include "ant_renderer.h"
#include "scene.h"
#include "ag_vdebug.h"
#include "ag_config.h"
#include "ag_fbo.h"
#include "ag_kill.h"

Renderer *gRenderer=0;

bool usePlainGL=true;

Renderer::Renderer():
  mCanMultitexture(-1),
  mCanShadow(-1),
  mCanGLSL(-1)
  {
    mFBO=0;
    CTRACE;
    GLeeInit(); // this call is essential for letting glee work
    assert(gRenderer==0);
    gRenderer=this;
    mScene=0;

    AGString shadowType=getConfig()->get("shadowType");
    if(shadowType=="big" || shadowType=="")
      shadowMapSize=1024;
    else if(shadowType=="small")
      shadowMapSize=512;
    else
      shadowMapSize=0;

    switch(shadowMapSize)
    {
    case 0:
      getConfig()->set("shadowType","none");break;
    case 512:
      getConfig()->set("shadowType","small");break;
    case 1024:
      getConfig()->set("shadowType","big");break;
    }


    shadowInited=false;

    if(!GLEE_EXT_texture3D)
      std::cerr<<"Sorry, Battles of Antargis won't work properly without GL_EXT_texture3D extension!"<<std::endl;
  }

Renderer::~Renderer()
  {
    cdebug("gRenderer:"<<gRenderer);
    assert(gRenderer==this);
    gRenderer=0;
    checkedDelete(mFBO); // checked - no agrubyobject
  }

bool Renderer::canMultitexture()
  {
    if(mCanMultitexture<0)
      mCanMultitexture=GLEE_ARB_multitexture;
    return mCanMultitexture;
  }

bool Renderer::canShadow()
  {
    if(mCanShadow<0)
      mCanShadow=(GLEE_ARB_shadow && GLEE_ARB_shadow_ambient);
    return mCanShadow && shadowMapSize>0;
  }

bool Renderer::canGLSL()
  {
    if(mCanGLSL<0)
      mCanGLSL=(GLEE_ARB_vertex_shader && GLEE_ARB_fragment_shader && GLEE_ARB_shading_language_100);
    return mCanGLSL;
  }

void Renderer::setCurrentScene(Scene *scene)
  {
    mScene=scene;
  }

Scene *Renderer::getCurrentScene()
  {
    assert(mScene);
    return mScene;
  }

GLint Renderer::getShadowUnit()
  {
    assert(canMultitexture());
    return 1;
  }
GLint Renderer::getNormalUnit()
  {
    if(canMultitexture())
      return 0;
    else
      return 0;
  }

void Renderer::initShadowTexture()
  {
    if(!canShadow())
      return;

    if(shadowInited)
      return;

    CTRACE;
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    //  glTexImage2D(   GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
    //  GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(   GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowMapSize, shadowMapSize, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    assertGL;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

    shadowInited=true;

    if(canFBO())
      {
        mFBO=new AGFBO(shadowMapTexture,shadowMapSize,shadowMapSize);//,true);
      }

  }

void Renderer::beginShadowComputation()
  {
    if(!shadowInited)
      initShadowTexture();

    if(mFBO)
      {
        mFBO->beginDraw();
      }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    assertGL;


    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(getCurrentScene()->getLightProj());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(getCurrentScene()->getLightView());

    //Use viewport the same size as the shadow map
    glViewport(0, 0, shadowMapSize, shadowMapSize);

    //Draw back faces into the shadow map
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    //Disable color writes, and use flat shading for speed
    glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 0);

    //Draw the scene
    // Offset the drawing a little back, so that slopy surfaces don't get shadowed
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1); // was 1,1

    assertGL;

  }
void Renderer::endShadowComputation()
  {
    glDisable(GL_POLYGON_OFFSET_FILL);
    assertGL;
    //Read the depth buffer into the shadow map texture
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    assertGL;

    /// @todo switch to FBOs - this should bring an improvement of 17 to 19 fps

    if(mFBO)
      {
        mFBO->endDraw();
      }
    else
      {
        glReadBuffer(GL_BACK);

        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, shadowMapSize, shadowMapSize,0);

        glReadBuffer(GL_FRONT);
      }

    assertGL;


    //restore states
    assertGL;
    glCullFace(GL_BACK);

    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);
  }


void Renderer::beginShadowDrawing()
  {
    assertGL;
    glActiveTexture(getShadowUnit());
    cdebug(getShadowUnit());
    assertGL;

    glMatrixMode(GL_MODELVIEW);
    // draw a flat shadow over 
    //  glDisable(GL_LIGHTING);
    //  glEnable(GL_COLOR_MATERIAL);
    assertGL;


    if(usePlainGL)
      {
        //Calculate texture matrix for projection
        //This matrix takes us from eye space to the light's clip space
        //It is postmultiplied by the inverse of the current view matrix when specifying texgen
        float bias[]={0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f};        //bias from [-1, 1] to [0, 1]
        static AGMatrix4 biasMatrix(bias);
        AGMatrix4 textureMatrix=biasMatrix * getCurrentScene()->getLightProj() * getCurrentScene()->getLightView();

        //Set up texture coordinate generation.
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

    //Bind & enable shadow map texture
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glEnable(GL_TEXTURE_2D);

    //Enable shadow comparison
    assertGL;

    if(usePlainGL)
      {

#ifndef OLD_SHADOW
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
        assertGL;
        //Shadow comparison should be true (ie not in shadow) if r<=texture
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL); // not needed ???
        assertGL;

        //Shadow comparison should generate an INTENSITY result
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
        assertGL;
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);//INTENSITY);
        assertGL;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 1.0f-0.3f);//shadowAlpha);
        assertGL;
        //      glColor4f(0,0,0,0.3);
        assertGL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#else
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);  //set this to a "depth texture"
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.5f);      //set the compare fail value

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);



        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL); // not needed ???
        assertGL;

        //Shadow comparison should generate an INTENSITY result
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
        assertGL;
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);//INTENSITY);



        // Set up the eye plane for projecting the shadow map on the scene

#endif
        //Set alpha test to discard false comparisons
        //  glAlphaFunc(GL_GREATER,0.99f);//LEQUAL, 0.999f);
        //      glAlphaFunc(GL_GEQUAL, 0.99f);
        //      glDisable(GL_ALPHA_TEST);


      }

    glActiveTexture(getNormalUnit());
    assertGL;

  }
void Renderer::endShadowDrawing()
  {
    assertGL;

    glDisable(GL_POLYGON_OFFSET_FILL);

    glActiveTexture(getShadowUnit());
    glBindTexture(GL_TEXTURE_2D,0);

    if(usePlainGL)
      {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
      }
    glActiveTexture(getNormalUnit());
    assertGL;

  }


bool Renderer::badShadowMap()
  {
    return shadowMapSize==1024;
  }


Renderer *getRenderer()
  {
    if(!gRenderer)
      {
        gRenderer=new Renderer;
        REGISTER_SINGLETON(gRenderer);
      }
    assert(gRenderer);
    return gRenderer;
  }
