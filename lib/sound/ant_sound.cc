#include "ant_sound.h"
#include "ag_config.h"
#include "ag_mixer.h"
#include "ag_vdebug.h"
#include "rk_debug.h"
#include "rk_string.h"
#include "scene_base.h"

#include <map>
namespace AntSound {
// this variables are used for playing sounds, which should be done with
// AntRubyEntity::playSound
const int INNER_VOL_SIZE =
    4; // size of circle around camera-middle with max volume
const int OUTER_VOL_SIZE =
    15; // size circle around camera describing volume descend

struct LoopData {
  int handle;
  AGString name;
  AGVector2 pos;
  float volume;
};
std::map<AGString, float> soundLastCall;
bool soundInited = false;
std::map<int, LoopData> loopSounds;
int ambientSound = 0;
int ambientMusic = 0;
SceneBase *sceneBase = 0;
bool normalVolume = true;

float getVolume(AGString type);
void setNormalVolumeWave();
void setApplication(SceneBase *psceneBase) { sceneBase = psceneBase; }

void initAntSound() {
  if (!soundInited) {
    setNormalVolumeWave();
    soundInited = true;
  }
}
void playSoundGlobal(const AGString &name, float volume, float minDiff) {
  std::map<std::string, std::vector<std::string>> sounds{
      {"sheep", {"data/sound/sheep44.wav"}},
      {"error", {"data/sound/error.wav"}},
      {"hackaxe", {"data/sound/tree_chop.wav"}},
      {"wind", {"data/sound/wind.wav"}},
      {"pick", {"data/sound/new_pick.wav"}},
      {"sword", {"data/sound/sword1.wav"}},
      {"fire", {"data/sound/fire.wav"}},
      {"ugh_end", {"data/sound/ugh_end.wav"}},
      {"won", {"data/sound/yay.wav"}},
      {"mylord", {"data/sound/yes_mylord.wav"}},
      {"ugh",
       {"data/sound/person_ugh2.wav", "data/sound/person_ugh4.wav",
        "data/sound/ugh.wav"}},
      {"die",
       {"data/sound/person_ugh1.wav", "data/sound/person_ugh3.wav",
        "data/sound/ugh2.wav"}},
      {"construct", {"data/sound/workshop.wav"}}};

  if (sounds.find(name) != sounds.end()) {
    // check if sound can already be played again
    if (soundLastCall.find(name) == soundLastCall.end()) {
      // was never played before, so init
      soundLastCall[name] = -minDiff - 1;
    }
    std::vector<std::string> &soundFiles = sounds[name];
    // select some random sound from list
    int i = rand() % soundFiles.size();
    initAntSound();
    getSoundManager()->playWave(soundFiles[i], volume);
  } else {
    cdebug("WARNING: Sound not found for: " << name);
  }
}

int playLoopSoundGlobal(int id, AGString name, const AGVector2 &pos,
                        float volume) {
  if (!sceneBase)
    return -1;

  std::map<std::string, std::vector<std::string>> sounds{
      {"fire", {"data/sound/fire.wav"}}};
  AGString s = sounds[name][0];
  if (s.length() > 0) {

    auto d = ((sceneBase->getCamera().dim2() - pos).length() - INNER_VOL_SIZE);
    float vol = 1;
    if (d > 0)
      vol = std::max((OUTER_VOL_SIZE - d) / OUTER_VOL_SIZE, 0.0f);
    auto handle = getSoundManager()->loopPlay(s, volume * vol);
    LoopData loopData;
    loopData.handle = handle;
    loopData.name = name;
    loopData.pos = pos;
    loopData.volume = volume;
    loopSounds[id] = loopData;
    return id;
  }
  return -1;
}

void stopLoopSound(int id) {
  if (loopSounds.find(id) == loopSounds.end())
    return;
  LoopData a = loopSounds[id];
  getSoundManager()->stopChannel(a.handle);
  loopSounds.erase(id);
}

void updateSoundPos(SceneBase *scene) {
  for (auto &entry : loopSounds) {
    auto value = entry.second;
    auto pos = value.pos;
    auto d = ((scene->getCamera().dim2() - pos).length() - INNER_VOL_SIZE);
    float vol = 1;
    if (d > 0)
      vol = std::max((OUTER_VOL_SIZE - d) / OUTER_VOL_SIZE, 0.0f);
    getSoundManager()->volumeSound(value.handle, value.volume * vol);
  }
}

void setAmbientSound(float time) {
  if (!sceneBase)
    return;
  if (!ambientSound) {
    ambientSound = getSoundManager()->loopPlay("data/sound/wind_loop.wav",
                                               getVolume("ambient") * 0.2);
    getSoundManager()->stopMp3();
    ambientMusic = getSoundManager()->playMp3("data/music/in-game1.ogg");
    getSoundManager()->volumeMusic(getVolume("music"));
  }
  if (!getSoundManager()->isMusicPlaying())
    ambientMusic = getSoundManager()->playMp3("data/music/in-game1.ogg");
}

void setNormalVolumeWave() {
  getSoundManager()->volumeSound(0.4 * getVolume("sound"));
  getSoundManager()->volumeMusic(getVolume("music"));
  normalVolume = true;
}

void setQuietVolumeWave() {
  getSoundManager()->volumeSound(0.1 * getVolume("sound"));
  getSoundManager()->volumeMusic(0.25 * getVolume("music"));
  normalVolume = false;
}

void updateVolumes() {
  getSoundManager()->volumeMusic(getVolume("music"));
  if (normalVolume)
    setNormalVolumeWave();
  else
    setQuietVolumeWave();

  if (ambientSound) {
    getSoundManager()->stopChannel(ambientSound);
    ambientSound = 0;
  }
  setAmbientSound(1);
}

float getVolume(AGString type) {
  auto v = getConfig()->get(type + "Volume");
  if (v == "") {
    if (type == "music")
      return 0.2;
    else
      return 1;
  }

  return v.toFloat();
}
} // namespace AntSound
