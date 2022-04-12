#ifndef __GENERATOR_GENERATOR_H
#define __GENERATOR_GENERATOR_H

#include "fluid_simulation.h"
#include "simulation_state.h"

namespace Generator {

class Generator {
  int w;
  SimulationState state;
  FluidSimulation sim;
  float dt;

public:
  Generator();
  void update();

  Grid2D<float> &terrain();
  Grid2D<float> &water();
};

} // namespace Generator

#endif
