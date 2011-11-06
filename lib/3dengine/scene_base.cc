#include "ag_profiler.h"

#include "quadtree.h"

#include "scene_base.h"

#include <algorithm>

SceneBase::SceneBase(int w,int h):
  mTree(new QuadTree<SceneNode>(AGRect2(AGVector2(),AGVector2(w,h)))),
  mCamera(w,h)
  {
  }


SceneBase::~SceneBase() throw()
  {
    // tell nodes, that I'm no longer there :-)
    for(Nodes::iterator i=mNodes.begin();i!=mNodes.end();i++)
      (*i)->resetScene(); 

    checkedDelete(mTree);
  }





void SceneBase::addNode(SceneNode *node)
  {
    //  std::cout<<"addNode:(this:"<<this<<") "<<node<<"  "<<typeid(*node).name()<<std::endl;
    if(mNodeSet.find(node)==mNodeSet.end())
      {
        node->setScene(this);

        mNodes.push_back(node);
        mNodeSet.insert(node);
        assert(node->getScene()==this);
        mTree->insert(node);
      }
  }



void SceneBase::updatePos(SceneNode *node)
  {
    if(mNodeSet.find(node)==mNodeSet.end())
      throw std::string("Dont know about this!");
    mTree->insert(node);
  }

void SceneBase::prepareUpdate(SceneNode *node)
  {
    if(mNodeSet.find(node)==mNodeSet.end())
      throw std::string("Dont know about this!");
    mTree->remove(node);
  }


void SceneBase::removeNode(SceneNode *node)
  {
    //  std::cout<<"remove node:"<<node<<std::endl;
    if(mNodeSet.find(node)!=mNodeSet.end())
      {
        Nodes::iterator i=std::find(mNodes.begin(),mNodes.end(),node);
        mNodes.erase(i);
        mNodeSet.erase(node);
        assert(node->getScene()==this);
        node->resetScene();
        bool ok=(mTree->remove(node));
        assert(ok);
      }
    else
      {
        throw std::runtime_error("Trying to remove unknown node");
      }
  }

void SceneBase::clear() throw()
  {
    for(std::vector<SceneNode*>::iterator i=mNodes.begin();i!=mNodes.end();i++)
      {
        assert((*i)->getScene()==this);
        (*i)->resetScene();
      }
    TRACE;
    mNodes.clear();
    mNodeSet.clear();
    mTree->clear();
  }

// (mx,my,0)
void SceneBase::setCamera(AGVector4 v)
  {
    mCamera.setPosition(v.dim3());
  }

void SceneBase::advance(float time)
  {
    STACKTRACE; 

    //  if(!mEnabled)
    //    return;
    // advance only in view

    SceneNodeList l=getCurrentNodes();

    for(SceneNodeList::iterator i=l.begin();i!=l.end();i++)
      {
        if((*i)->visible())
          {
            (*i)->advance(time);
          }
      }
  }

float SceneBase::width() const
{
  return mCamera.getWidth();
}
float SceneBase::height() const
{
  return mCamera.getHeight();
}

AGVector4 SceneBase::getCamera() const
{
  return AGVector4(mCamera.getPosition(),1);
}

AntCamera &SceneBase::getCameraObject()
  {
    return mCamera;
  }

SceneNodeList SceneBase::getCurrentNodes()
  {
    throw std::runtime_error("not implemented!");
    return SceneNodeList();
  }
