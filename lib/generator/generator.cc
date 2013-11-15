#include "generator.h"

namespace Generator {

  Generator::Generator():w(32*8),state(w,w,time(0)),sim(state),dt ( 1000.0/(60)) {
  }

  void Generator::update() {
    sim.update(dt,true,false);
  }

  Grid2D<float> &Generator::terrain() {
    return state.terrain;
  }
  Grid2D<float> &Generator::water() {
    return state.water;
  }
}
