#ifndef AG_VIDEO_BASE_H
#define AG_VIDEO_BASE_H

#include <rk_base.h>

class AGEXPORT AGVideoBase {
public:
  virtual ~AGVideoBase() throw();
  virtual int width() const;
  virtual int height() const;

  virtual int realWidth() const;
  virtual int realHeight() const;

  virtual void initVideo(int w, int h, int d, bool fs, bool gl, int vw = -1,
                         int vh = -1);
};

#endif
