#ifndef AG_SOUND_H
#define AG_SOUND_H

class AGString;
class AGVector2;
class SceneBase;

namespace AntSound {

void setApplication(SceneBase *psceneBase);
void playSoundGlobal(const AGString &name, float volume, float minDiff = 0.5);
int playLoopSoundGlobal(int id, AGString name, const AGVector2 &pos,
                        float volume);
void stopLoopSound(int id);
void updateSoundPos(SceneBase *scene);
} // namespace AntSound

#endif
