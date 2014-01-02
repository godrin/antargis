#include "gl_app.h"
#include "ag_vdebug.h"
#include "ag_profiler.h"
#include "ant_sound.h"

GLApp::GLApp(int w,int h):scene(new Scene(w,h))
{
  shadow=true;
  mx=my=0;
  omx=-1;
  frameTime=0;
  frameCount=0;
  hx=hy=-1;
  AntSound::setApplication(scene);
}

GLApp::~GLApp() throw()
  {
    checkedDelete(scene);
  }


void GLApp::draw()
  {
    STACKTRACE;
    drawGL();
    AGApplication::draw();
  }

void GLApp::drawGL()
  {
    STACKTRACE;
    glEnable(GL_DEPTH_TEST); // enable depth test
    glDepthMask(true);

    assertGL;
    scene->draw();
    assertGL;
  }

bool GLApp::eventFrame(float t)
  {
    if(hx>=0)
      {
        // check hovering
        PickResult nodes=scene->pick(hx,hy,1,1);

        if(nodes.size())
          eventHover(nodes,hb);
        hx=hy=-1;
      }
    return true;
  }

bool GLApp::eventMouseButtonDown(AGEvent *e)
  {
    mMayClick=true;

    if(e->isSDLEvent())
      {
        if(e->getButton()==5)
          {
            getScene().getCameraObject().incCameraDistance();
          }
        else if(e->getButton()==4)
          {
            getScene().getCameraObject().decCameraDistance();
          }
        else if(e->getButton()==1)
          {
            AGVector2 p=e->getMousePosition();
            omx=p[0];
            omy=p[1];
          }
      }
    return AGApplication::eventMouseButtonDown(e);
  }
bool GLApp::eventMouseButtonUp(AGEvent *e)
  {
    omx=-1;
    omy=-1;
    if(mMayClick)
      {
        if(e->isSDLEvent())
          {
            if(e->getButton()<4)
              {
                STACKTRACE;
                AGVector2 p=e->getMousePosition();
                cdebug("p:"<<p);
                PickResult nodes=scene->pick(p[0],p[1],1,1);

                eventClick(nodes,e->getButton());
              }
          }
      }
    else
      {
        // no click, because mouse not down
      }
    return AGApplication::eventMouseButtonUp(e);
  }

void GLApp::eventClick(const PickResult &pNodes,int button)
  {
  }

void GLApp::eventHover(const PickResult &pNodes,int button)
  {
  }


bool GLApp::eventMouseMotion(AGEvent *e)
  {
    if(e->isSDLEvent() && omx>=0)
      {
        AGVector2 p=e->getMousePosition();
        //      AGVector4 cam=scene->getCamera();
        mx=p[0]-omx;
        my=p[1]-omy;

        setCamera(getCamera()+AGVector2(-mx*0.03,my*0.03));
        //      camera=cam+AGVector4(-mx*0.03,my*0.03,0);
        //      scene->setCamera(camera);

        omx=p[0];
        omy=p[1];
      }
    if(e->isSDLEvent())
      {
        // check hovering - delayed 
        AGVector2 p=e->getMousePosition();
        hx=p[0];
        hy=p[1];
        hb=e->getButton();
      }

    mMayClick=false;
    return AGApplication::eventMouseMotion(e);
  }

AGVector2 GLApp::getCamera() const
{
  return scene->getCamera().dim2();
}

void GLApp::setCamera(const AGVector2 &p)
  {
    scene->setCamera(AGVector4(p[0],p[1],getCameraHeight(p)));
    AntSound::updateSoundPos(scene);
  }

bool GLApp::eventKeyDown(AGEvent *e)
  {
    if(e->isSDLEvent())
      {
        if(e->getKey()==SDLK_s)
          {
            scene->setShadow((scene->getShadow()+1)%3);
          }
      }
    return AGApplication::eventKeyDown(e);
  }

Scene &GLApp::getScene()
  {
    assert(scene);
    return *scene;
  }

float GLApp::getCameraHeight(const AGVector2&p)
  {
    return 0;
  }
