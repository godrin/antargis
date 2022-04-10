/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef STATE_H
#define STATE_H

#include "grid_2d.h"

#include "perlin_noise.h"

//#include <glm.hpp>
#include <iostream>

using namespace glm;

class SimulationState
{
public:
    Grid2D<float> water;
    Grid2D<float> terrain;
    Grid2D<float> rock;
    Grid2D<float> suspendedSediment;

    Grid2D<vec3> surfaceNormals;
public:

    SimulationState(uint w, uint h, uint seed)
        :   water(w,h),
            terrain(w,h),
            rock(w,h),
            suspendedSediment(w,h),
            surfaceNormals(w,h)
    {

      createPerlinTerrain(seed,terrain);
      createPerlinTerrain(seed,rock);
      zeroGrid(water);
      zeroGrid(suspendedSediment);

      int l = 20;
      int mw = 20;
      int xmin = water.width() - l-mw;
      int xmax = water.width() - l;

      int ymin = water.height() - l-mw;
      int ymax = water.height() - l;

    }

    void createPerlinTerrain(uint seed,Grid2D<float> &grid,float heightScale=4)
    {
      std::cout<<"SEED:"<<seed<<std::endl;
      PerlinNoise perlin(seed);

      for (uint y=0; y<grid.height(); y++)
      {
        for (uint x=0; x<grid.width(); x++)
        {
          float h = 0.0f; float f = 0.05f;
          h += perlin.Sample(y*f,x*f)*1; f /= 2;
          h += perlin.Sample(y*f,x*f)*2; f /= 2;
          h += perlin.Sample(y*f,x*f)*4; f /= 2;
          h += perlin.Sample(y*f,x*f)*8; f /= 2;
          grid(y,x) = h*heightScale*1.3;
          suspendedSediment(y,x) = 0.0f;// 0.1*terrain(y,x);

        }
      }
    }

    void zeroGrid(Grid2D<float> &grid) {
      for (uint y=0; y<grid.height(); y++)
      {
        for (uint x=0; x<grid.width(); x++)
        {
          grid(y,x) = 0.0f;
        }
      }

    }

    void createSteepTerrain()
    {
      for (uint y=0; y<water.height(); y++)
      {
        for (uint x=0; x<water.width(); x++)
        {
          water(y,x) = 0.0f;
          if (x > (water.height()/2)) {
            terrain(y,x) = 0.2*(x-water.height()/2);
          } else {
            terrain(y,x) = 0.2*(-x+water.height()/2);
          }
          if (y > (water.height()/2)) {
            terrain(y,x) = std::max(terrain(y,x),0.2f*(y-water.height()/2));
          } else {
            terrain(y,x) = std::max(terrain(y,x),0.2f*(-y+water.height()/2));
          }
          suspendedSediment(y,x) = 0.0f;
        }
      }
    }


};

#endif // STATE_H
