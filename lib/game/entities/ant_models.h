#ifndef __ANT_MODELS_H
#define __ANT_MODELS_H

#include <rk_string.h>

class SceneNode;
class Scene;

class AntModels {
public:
  static SceneNode *createModel(Scene *scene,std::string type,std::string subtype="",float angle=0);
};

#endif
