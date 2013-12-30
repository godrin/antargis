#ifndef AG_SOUND_H
#define AG_SOUND_H

class AGString;
class SceneBase;

namespace  AntSound {

  void setApplication(SceneBase *psceneBase);
  void playSoundGlobal(AGString name,float volume,float minDiff=0.5);
}

#endif

