#ifndef __GENERATOR_VIEWER_H
#define __GENERATOR_VIEWER_H

#include "grid_2d.h"
class SimulationState;

namespace Generator {

class Map2D;
void view(Map2D &map);

void view(const Grid2D<float> &grid, float from, float to, int stepping);
void view(const SimulationState &state, float from, float to, int step,
          int frame);

void close();
char waitForKey();

} // namespace Generator

#endif
