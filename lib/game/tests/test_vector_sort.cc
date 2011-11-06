#include "path_vector_sort.h"
#include <ag_geometry.h>
#include <set>
#include <ag_debug.h>

bool test_vector_sort()
  {
    AGVector2 m(2,2);
    DistanceOrder order(m);
    std::set<AGVector2,DistanceOrder> mset(order);

    AGVector2 a(3,2);
    AGVector2 b(3,3);
    AGVector2 c(0,0);
    AGVector2 d(1,2);


    mset.insert(a);
    mset.insert(b);
    mset.insert(c);
    mset.insert(d);


    std::vector<AGVector2> v;

    std::copy(mset.begin(),mset.end(),std::back_inserter(v));

    /*
  cdebug(v.size());

  cdebug("0:"<<v[0]<<"  "<<d<<"  "<<(d-m).length());
  cdebug("1:"<<v[1]<<"  "<<a<<"  "<<(a-m).length());
  cdebug("2:"<<v[2]<<"  "<<b<<"  "<<(b-m).length());
  cdebug("3:"<<v[3]<<"  "<<c<<"  "<<(c-m).length());
     */
    return false;
    assert(v[0]==d);
    assert(v[1]==a);
    assert(v[2]==b);
    assert(v[3]==c);

    return true;
  }


bool test_my_test()
  {
    return true;
  }
