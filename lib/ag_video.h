#ifndef AG_VIDEO_H
#define AG_VIDEO_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_video_base.h>

#include <string>
class AGScreen;

class AGEXPORT AGVideoManager:public AGVideoBase
{
 public:
  AGVideoManager();

  void flip();
  /** change screen resolution to w*h*d with
      fullscreen when fs is true and in opengl mode if gl is true
      vw and vh describe virtual screen resolutions which implies scaling
      This is supported in opengl-mode only.
   */
  void initVideo(int w,int h,int d,bool fs,bool gl,int vw=-1,int vh=-1);
  void toggleFull();

  int width() const;
  int height() const;

  int realWidth() const;
  int realHeight() const;

  void setIcon(const std::string &pFile);
  void setCaption(const std::string &pCaption);

  bool fullscreen() const;


 private:
  AGScreen *mScreen;

};

//bool glMode();
AGEXPORT bool opengl();

AGEXPORT AGVideoManager *getVideo();

#endif


