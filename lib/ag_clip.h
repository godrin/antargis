#ifndef AG_CLIP_H
#define AG_CLIP_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_geometry.h>

class AGEXPORT AGClipping
{
 public:
  void include(const AGRect2 &r);
  void exclude(const AGRect2 &r);
  
  std::vector<AGRect2> clip(const AGRect2&r);
  std::vector<std::pair<AGRect2,AGRect2> > clip(const AGRect2&r,const AGRect2 &sync);

  std::vector<AGLine2> clip(const AGLine2 &p);

  bool included(const AGVector2 &v);

  bool valid() const;

  AGString toString() const;

 private:

  void optimize();

  //  std::vector<AGRect2> mInclude;
  std::vector<AGRect2> mExclude;
};

#endif
