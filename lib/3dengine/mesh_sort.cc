#include "mesh_sort.h"
#include "rk_debug.h"


bool SortDistance::operator()(const SceneNode *n1,const SceneNode *n2)
{
  //cdebug("SORTDIST "<<n1<<" "<<n2);
  AGVector3 m1=n1->bbox().base+n1->bbox().dir*0.5;
  AGVector3 m2=n2->bbox().base+n2->bbox().dir*0.5;
  
  return (m1-cam).length2()<(m2-cam).length2();
}

bool SortOrder::operator()(const SceneNode *n1,const SceneNode *n2)
{
  return n1->getOrder()<n2->getOrder();
}



bool SortYCoord::operator()(const SceneNode *n1,const SceneNode *n2)
{
  //  return n1->getPos()[1]>n2->getPos()[1];
  return n1->getOrder()<n2->getOrder() || (n1->getOrder()==n2->getOrder() && n1->getPos()[1]>n2->getPos()[1]);
}
