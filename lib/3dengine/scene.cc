#include "ant_renderer.h"
#include "scene.h"

#include <SDL_opengl.h>

#include <rk_debug.h>
#include <ag_vdebug.h>
#include <ag_video.h>
#include <ag_rendercontext.h>
#include <algorithm>

#include <cmath>

#include "quadtree.h"
#include "ag_profiler.h"
#include "ag_main.h"
#include "mesh_sort.h"

bool PickNode::operator<(const PickNode &n) const
{
  return camDist<n.camDist;
}



Scene::Scene(int w,int h):
  SceneBase(w,h)
  {
    white=AGVector4(1,1,1,1);
    black=AGVector4(0,0,0,1);

    GLeeInit();

    cdebug("SHADOW:"<<(int)GLEE_ARB_shadow);
    cdebug("SHADOW_AMB:"<<(int)GLEE_ARB_shadow_ambient);

    if(getRenderer()->canShadow())
      mShadow=1;
    else
      mShadow=0;

    mEnabled=true;
  }


Scene::~Scene() throw()
  {
  //throw int();
  }

size_t Scene::getDrawnMeshes() const
{
  return mMeshes;
}

size_t Scene::getTriangles() const
{
  return mTriangles;
}

size_t Scene::getPickTriangles() const
{
  return mPickTriangles;
}


void Scene::draw()
  {
    if(!mEnabled)
      return;
    AGRenderContext c;
    c.begin(); // reset gl-state

    getRenderer()->setCurrentScene(this);
    assertGL;

    mMeshes=0;
    mTriangles=0;
    mPickTriangles=0;

    for(Nodes::iterator i=mNodes.begin();i!=mNodes.end();i++)
      (*i)->sort(AGVector4(mCamera.getPosition(),1));

    if(mShadow)
      {
        calcShadowMap();
        initScene();
        drawShadow();
      }
    else
      {
        initScene();
        drawScene();
      }

    getRenderer()->setCurrentScene(0);
  }

void Scene::setShadow(int v)
  {
    if(getRenderer()->canShadow())
      {
        mShadow=v;
        cdebug(mShadow);
      }
  }
int Scene::getShadow() const
{
  return mShadow;
}




SceneNodeList Scene::getCurrentNodes()
  {
    STACKTRACE;
    AGVector2 p=mCamera.getPosition().dim2();
    SceneNodeList l=mTree->get(AGRect2(p+AGVector2(-30,-30),p+AGVector2(30,30)));

    for(SceneNodeList::iterator i=l.begin();i!=l.end();i++)
      {
        if(mNodeSet.find(*i)==mNodeSet.end())
          cdebug("ERROR:"<<*i);
        assert(mNodeSet.find(*i)!=mNodeSet.end());
      }
    return l;
  }


void Scene::calcShadowMap()
  {
    STACKTRACE;
    assertGL;
    //  AGMatrix4 frustum=getFrustum();
    size_t shadowMeshes=0;

    getRenderer()->beginShadowComputation();

      {
        STACKTRACE;
        SceneNodeList l=getCurrentNodes();
        Nodes sorted;
        std::copy(l.rbegin(),l.rend(),std::back_inserter(sorted));


          {
            // apply frustrum culling
            AntFrustum cFrustum=mCamera.getCameraProjection().getFrustum();
            for(Nodes::iterator i=sorted.begin();i!=sorted.end();)
              {
                if(cFrustum.collides((*i)->bbox()))
                  i++;
                else
                  i=sorted.erase(i);
              }
          }

          sort(sorted.begin(),sorted.end(),SortOrder());

          for(Nodes::iterator i=sorted.begin();i!=sorted.end();i++)
            {
                {
                  if((*i)->visible())
                    {
                      (*i)->drawDepth();
                      mTriangles+=(*i)->getTriangles();
                      shadowMeshes++;
                    }
                }
            }
      }
      //  cdebug("shadowMeshes:"<<shadowMeshes);

      getRenderer()->endShadowComputation();
      assertGL;
  }

/**
   setups up lighting and gl-matrices (projection and such)
 */
void Scene::initScene()
  {
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(mCamera.getProjection());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float xfactor=float(getVideo()->realWidth())/getVideo()->width();
    float yfactor=float(getVideo()->realHeight())/getVideo()->height();

    glViewport(0, 0,(GLint)(mCamera.getWidth()*xfactor), (GLint)(mCamera.getHeight()*yfactor));

    //Use dim light to represent shadowed areas

    AGVector4 l=mCamera.getLightPosition();
    l[3]=1;

    glLightfv(GL_LIGHT1, GL_POSITION, l);
    glLightfv(GL_LIGHT1, GL_AMBIENT, AGVector4(0.1,0.1,0.1,1));//white*0.05f);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, AGVector4(0.3,0.3,0.3,1));//white*0.3f);//*0.2f);
    glLightfv(GL_LIGHT1, GL_SPECULAR, black);
    glEnable(GL_LIGHT1);

    AGVector4 lightPosition2=AGVector4( 0, 0, -50,1);

    //  glLightfv(GL_LIGHT2, GL_POSITION, lightPosition2+scenePosition);
    glLightfv(GL_LIGHT2, GL_AMBIENT, black);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, AGVector4(0.7,0.7,0.7,1));//white*0.7f);//*0.2f);
    glLightfv(GL_LIGHT2, GL_SPECULAR, white);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, AGVector4(0,0,-1,0));
    glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION,0);
    glLightf(GL_LIGHT2,GL_QUADRATIC_ATTENUATION,0);
    glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION,0);
    glEnable(GL_LIGHT2);



    glEnable(GL_LIGHTING);

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT3);
    glDisable(GL_LIGHT4);
    glDisable(GL_LIGHT5);

    //  glLoadMatrixf(cameraViewMatrix);
    glLoadMatrixf(mCamera.getModelview());
  }

#ifdef TEST_DL
static GLuint displayList=0;
static bool dlInited=false;
#endif

void Scene::drawScene()
  {
#ifdef TEST_DL
    // this was for testing only - to check if display-lists are any good.
    // it seems they are comparable with VBOs - the problem is that they're inflexible
    // please leave this code, so we have some example, if we happen to use this again at a later stage

    if(dlInited)
      {
        glCallList(displayList);
      }
    else
      {
        dlInited=true;
        displayList=glGenLists(1);
        glNewList(displayList,GL_COMPILE);
#endif

        STACKTRACE;

        // this is used for frustum cullin
        AntFrustum cFrustum=mCamera.getCameraProjection().getFrustum();

        //2nd pass - Draw from camera's point of view

        // draw scene with texturing and so

        int drawn=0;

        SceneNodeList l=getCurrentNodes();
        Nodes sorted;
        std::copy(l.begin(),l.end(),std::back_inserter(sorted));

          {
            STACKTRACE;
            for(Nodes::iterator i=sorted.begin();i!=sorted.end();)
              {
                if(cFrustum.collides((*i)->bbox()))
                  i++;
                else
                  i=sorted.erase(i);
              }
          }

          sort(sorted.begin(),sorted.end(),SortOrder());
          // draw opaque objects first, from front to back
          for(Nodes::iterator i=sorted.begin();i!=sorted.end();i++)
            {
              if(!(*i)->transparent())
                {
                  if((*i)->visible())
                    {
                      (*i)->draw();
                      mTriangles+=(*i)->getTriangles();
                      drawn++;
                      mMeshes++;
                    }
                }
            }
          sort(sorted.begin(),sorted.end(),SortDistance(mCamera.getCameraPosition().dim3()));
          // draw transparent ones next from back to front
          for(Nodes::reverse_iterator i=sorted.rbegin();i!=sorted.rend();i++)
            {
              if((*i)->transparent())
                {
                  if((*i)->visible())
                    {
                      (*i)->draw();
                      mTriangles+=(*i)->getTriangles();
                      drawn++;
                      mMeshes++;
                    }
                }
            }

#ifdef TEST_DL
          glEndList();
          glCallList(displayList);
      }
#endif

  }


/// deprecated function - this is done in one pass with "normal" drawing
/// it was used to paint the shadow afterwards in a 3rd pass
void Scene::drawShadow()
  {
    assertGL;

    getRenderer()->beginShadowDrawing();

    drawScene();

    getRenderer()->endShadowDrawing();

    assertGL;
  }

AGVector3 Scene::getCameraDirTo(const AGVector3 &p) const
{
  return mCamera.getCameraPosition().dim3()-p;
}

/** pickDraw is used for picking ;-)
    it draws all the objects with opengl
    and not using texturing, shaders and such - if I'm right here??
 */
void Scene::pickDraw()
  {
    STACKTRACE;
    GLuint name=1;
    pickNames.clear();

    AGMatrix4 frustum=cameraPickMatrix*mCamera.getModelview();

    AntFrustum cFrustum=mCamera.getCameraProjection().getFrustum();

    SceneNodeList l=getCurrentNodes();

    for(SceneNodeList::iterator i=l.begin();i!=l.end();i++)
      {
        STACKTRACE;
        if((*i)->visible() && (*i)->bbox().collides(frustum))
          if(cFrustum.collides((*i)->bbox()))
            {
              STACKTRACE;
              glPushName(name);
              (*i)->drawPick();
              glPopName();
              pickNames.insert(std::make_pair(name,*i));
              name++;
              mPickTriangles+=(*i)->getTriangles();
            }
      }

    glEnable(GL_CULL_FACE);
  }

/// this a global function - use this for picking!
/// x and y are in screen-coordinates in normal fashion
/// so (0,0) is the top left corner and (1023,767) bottom right.
/// the same for w and h
PickResult Scene::pick(float x,float y,float w,float h)
  {
    STACKTRACE;
    size_t bufsize=4000;
    GLuint buffer[bufsize+1];

    glSelectBuffer(bufsize,buffer);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    assertGL;
    gluPickMatrix(x,mCamera.getHeight()-y,h,w,getViewport());

    assertGL;

    glMultMatrixf(mCamera.getProjection());
    glGetFloatv(GL_PROJECTION_MATRIX, cameraPickMatrix);
    assertGL;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMultMatrixf(mCamera.getModelview());
    glInitNames();
    assertGL;

    pickDraw();
    assertGL;


    // back to normality
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glFlush();
    assertGL;

    int hits=glRenderMode(GL_RENDER);
    assertGL;
    PickResult r=processHits(hits,buffer,x+w/2,mCamera.getHeight()-(y+h/2));
    std::sort(r.begin(),r.end());
    return r;
  }

/// helper function for gettin PickResult from opengl's buffers
PickResult Scene::processHits (int hits, GLuint *buffer,float px,float py)
  {
    STACKTRACE;
    PickResult result;
    if(hits==0)
      return result;

    unsigned int i, j;
    GLuint names, *ptr, minZ,*ptrNames, numberOfNames;

    ptr = (GLuint *) buffer;
    minZ = 0xffffffff;
    for (i = 0; i < (unsigned int)hits; i++) {
      names = *ptr;
      ptr++;
      if (*ptr < minZ) {
        numberOfNames = names;
        minZ = *ptr;

        ptrNames = ptr+2;

        GLuint *mptr=ptrNames;
        for(j=0;j<numberOfNames;j++,mptr++)
          {
            PickNode n;
            n.node=pickNames[*mptr];
            n.camDist=minZ/float(0xFFFFFFFF); // (0-1)

            // get world-position
            GLdouble x,y,z;

            GLdouble modelview[16],projection[16];
            for(size_t i=0;i<16;i++)
              {
                modelview[i]=((float*)mCamera.getModelview())[i];
                projection[i]=((float*)cameraPickMatrix)[i];
              }

            gluUnProject(px,py,n.camDist,modelview,projection,getViewport(),&x,&y,&z);
            n.pos=AGVector4(x,y,z,1);
            n.camDist=(n.pos-mCamera.getCameraPosition()).length3();

            result.push_back(n);
          }

      }

      ptr += names+2;
    }

    return result;
  }


Viewport Scene::getViewport() const
{
  return mCamera.getViewport();
}



AGMatrix4 Scene::getLightComplete() const
{
  return mCamera.getLightComplete();
}

AGMatrix4 Scene::getLightView() const
{
  return mCamera.getLightView();
}
AGMatrix4 Scene::getLightProj() const
{
  return mCamera.getLightProjectionMatrix();
}


AGVector2 Scene::getPosition(const AGVector4 &v) const
{
  GLdouble x,y,z;

  GLdouble modelview[16],projection[16];
  for(size_t i=0;i<16;i++)
    {
      modelview[i]=((const float*)mCamera.getModelview())[i];
      projection[i]=((const float*)mCamera.getProjection())[i];
    }

  gluProject(v[0],v[1],v[2],modelview,projection,getViewport(),&x,&y,&z);
  return AGVector2((int)x,((int)mCamera.getHeight()-y));
}



void Scene::setEnabled(bool p)
  {
    mEnabled=p;
  }


void Scene::advance(float time)
  {
    if(mEnabled)
      SceneBase::advance(time);
  }
