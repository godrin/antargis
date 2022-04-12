#ifndef AG_RAND_BASE_H
#define AG_RAND_BASE_H

#include "rk_base.h"

class AGEXPORT AGRandomizerBase {
public:
  virtual float operator()(float f);
  virtual int operator()(int i);
};

#endif
