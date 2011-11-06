#ifndef SCENE_2D_H
#define SCENE_2D_H

#include "scene_base.h"

class AGPainter;

class AGEXPORT Scene2D:public SceneBase
{
 public:
  typedef std::vector<PickNode> PickResult;
  typedef std::list<SceneNode*> NodeList;

  Scene2D(int w,int h);
  virtual ~Scene2D() throw();

  void draw();

  /**
     picking is currently done with opengl. this uses software (at least on my box), which is
     pretty slow. Some new implementation using BSPs would be cool!
     VertexArray or MeshData should contain it's data in such a tree. rays can be transformed using
     inverse transformation-matrices. This way data can stay as is.
  */
  PickResult pick(float x,float y,float w,float h);

  //  AntCamera &getCameraObject();

  AGVector2 getPosition(const AGVector4 &v) const;

  NodeList getCurrentNodes();

  void setEnabled(bool p);

  void setPainter(AGPainter *p);
  void discardPainter();

  AGPainter *getPainter();
 private:
  AGPainter *mPainter;
  
  
  

};

#endif
