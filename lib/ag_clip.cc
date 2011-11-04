#include "ag_clip.h"
#include "ag_projection.h"
#include "ag_stringstream.h"
#include <stdexcept>

void AGClipping::exclude(const AGRect2 &r)
  {
    mExclude.push_back(r);
    //FIXME: check for intersections
  }

void AGClipping::include(const AGRect2 &r)
  {
    std::vector<AGRect2> n;

    for(std::vector<AGRect2>::iterator i=mExclude.begin();i!=mExclude.end();i++)
      {
        std::vector<AGRect2> t=(*i).difference(r);
        std::copy(t.begin(),t.end(),std::back_inserter(n));
      }
    mExclude=n;
  }

std::vector<AGRect2> AGClipping::clip(const AGRect2&r)
  {
    std::vector<AGRect2> t,n;

    t.push_back(r);

    for(std::vector<AGRect2>::iterator i=mExclude.begin();i!=mExclude.end();i++)
      {
        for(std::vector<AGRect2>::iterator j=t.begin();j!=t.end();j++)
          {
            std::vector<AGRect2> t2=j->difference(*i);
            for(std::vector<AGRect2>::iterator k=t2.begin();k!=t2.end();k++)
              {
                if(k->w()>0 && k->h()>0)
                  n.push_back(*k);
              }
          }
        t=n;
        n.clear();

      }

    // FIXME: check for optimization

    return t;

  }

bool AGClipping::included(const AGVector2 &v)
  {
    for(std::vector<AGRect2>::iterator i=mExclude.begin();i!=mExclude.end();i++)
      if(i->contains(v))
        return false;
    return true;
  }


std::vector<std::pair<AGRect2,AGRect2> > AGClipping::clip(const AGRect2&r,const AGRect2 &sync)
  {
    std::vector<std::pair<AGRect2,AGRect2> > n;

    std::vector<AGRect2> t=clip(r);

    AGProjection2D p(r,sync);

    for(std::vector<AGRect2>::iterator i=t.begin();i!=t.end();i++)
      {
        n.push_back(std::make_pair(*i,p.project(*i)));
      }

    return n;
  }

AGString AGClipping::toString() const
{
  AGStringStream os;

  os<<"[AGClipping:";
  for(std::vector<AGRect2>::const_iterator i=mExclude.begin();i!=mExclude.end();i++)
    os<<i->toString()<<";";
  os<<"]";

  return os.str();
}

std::vector<AGLine2> AGClipping::clip(const AGLine2 &p)
  {
#warning "implement me"
    std::vector<AGLine2> l;
    l.push_back(p);
    return l;
  }

bool AGClipping::valid() const
{
#warning "IMPLEMENT ME"
  return true;
}
