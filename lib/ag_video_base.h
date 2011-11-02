#ifndef AG_VIDEO_BASE_H
#define AG_VIDEO_BASE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_singleton.h>

class AGEXPORT AGVideoBase:public AGSingleton
{
 public:
  virtual ~AGVideoBase() throw();
  virtual int width() const;
  virtual int height() const;

  virtual int realWidth() const;
  virtual int realHeight() const;

};


#endif
