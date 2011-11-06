#ifndef NO

#include "scene_2d.h"
#include "mesh_2d.h"
#include "mesh_sort.h"
#include <rk_debug.h>

#include <algorithm>

Scene2D::Scene2D(int w,int h):
  SceneBase(w,h),mPainter(0)
  {
  }

Scene2D::~Scene2D() throw()
  {
  }

void Scene2D::draw()
  {
    // FIXME

    SceneNodeList nodeList=getCurrentNodes();
    Nodes ns;
    std::copy(nodeList.begin(),nodeList.end(),std::back_inserter(ns));


    sort(ns.begin(),ns.end(),SortYCoord());

    //FIXME:sort!!

    for(Nodes::iterator i=ns.begin();i!=ns.end();i++)
      (*i)->draw();
  }

PickResult Scene2D::pick(float x,float y,float w,float h)
  {
    PickResult result;
    SceneNodeList nodeList=getCurrentNodes();

    Nodes ns;
    std::copy(nodeList.begin(),nodeList.end(),std::back_inserter(ns));
    sort(ns.begin(),ns.end(),SortOrder());

    for(Nodes::reverse_iterator i=ns.rbegin();i!=ns.rend();i++)
      {
        Mesh2D*m=dynamic_cast<Mesh2D*>(*i);
        if(m)
          {
            if(m->hit(AGVector2(x,y)))
              {
                PickNode node;
                node.pos=m->getPos();
                node.node=m;
                node.camDist=0;

                cdebug("hit:"<<node.pos<<"   "<<node.node);
                result.push_back(node);
              }
          }
      }


    return result;
  }

AGVector2 Scene2D::getPosition(const AGVector4 &v) const
{
  // FIXME: include camera !!
  AGVector2 center(width()/2,height()/2);
  AGVector2 cam(mCamera.getPosition().dim2());

  AGVector2 n=v.dim2()-cam;

  //  cdebug("n:"<<n);

  //  cdebug("n:"<<n);
  n*=32;
  //  cdebug("n:"<<n);

  n+=center;


  return AGVector2(n[0],height()-n[1]);

  throw std::runtime_error("FIXME");
  return v.dim2();
}

SceneNodeList Scene2D::getCurrentNodes()
  {
    //  cdebug("FIXME");

    SceneNodeList l;
    std::copy(mNodes.begin(),mNodes.end(),std::back_inserter(l));

    //  throw std::runtime_error("FIXME");
    return l;
  }

//void setEnabled(bool p);

void Scene2D::setPainter(AGPainter *p)
  {
    CTRACE;
    assert(mPainter==0);
    mPainter=p;
  }
void Scene2D::discardPainter()
  {
    CTRACE;
    assert(mPainter!=0);
    mPainter=0;
  }

AGPainter *Scene2D::getPainter()
  {
    CTRACE;
    assert(mPainter);
    return mPainter;
  }


#endif
