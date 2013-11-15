#include <iostream>

#include "generator_map.h"
#include "generator_viewer.h"

#include "fluid_simulation.h"
#include "simulation_state.h"

#include <time.h>

int mainOld() {
  std::cout<<"XX"<<std::endl;

  srand (time(NULL));
  int low=8;
  int high=low*4-1;

  Generator::Map2D m2d0(low,low);
  Generator::Map2D m2d1(high,high);
  Generator::Map m(512,512,128);

  std::cout<<"low res 2d..."<<std::endl;
  Generator::forAll(m2d0,&Generator::nullify);
  Generator::forAll(m2d0,Generator::random(Generator::Cell::SAND,120));
 
  output(m2d0,Generator::Cell::SAND);
  std::cout<<"blur..."<<std::endl;
  Generator::blur(m2d0,Generator::Cell::SAND); 

  output(m2d0,Generator::Cell::SAND);

  std::cout<<"high res 2d..."<<std::endl;

  interpolate(m2d0,m2d1,Generator::Cell::SAND);
  output(m2d1,Generator::Cell::SAND);



  std::cout<<"some more detail..."<<std::endl;

  Generator::Map2D m2d2(high,high);
  Generator::forAll(m2d2,Generator::random(Generator::Cell::SAND,40));
  add(m2d1,m2d2,Generator::Cell::SAND);

  output(m2d1,Generator::Cell::SAND);

//  Generator::view(m2d1);



  std::cout<<"high res 3d..."<<std::endl;
  Generator::forAll(m,&Generator::nullify);
  return 0;
}

void output(Grid2D<float> grid) {
  for(int y=0;y<grid.height();y++) {
    for(int x=0;x<grid.width();x++) {
      printf("% 3.2f ",grid(y,x));
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
  std::cout<<std::endl;
}

void diff(Grid2D<float> &old,Grid2D<float> &n) {
  for(int y=0;y<old.height();y++) {
    for(int x=0;x<old.width();x++) {
      old(y,x)=old(y,x)-n(y,x);
    }
  }
}

void output(SimulationState &state) {
  std::cout<<"Terrain:"<<std::endl;
  output(state.terrain);
  std::cout<<"Water:"<<std::endl;
  output(state.water);
  std::cout<<"suspended:"<<std::endl;
  output(state.suspendedSediment);
}

int main() {


  int step=4;
  int w=32*step;

  SimulationState state(w,w,time(0));
  FluidSimulation sim(state);
  double dt = 1000.0/(60);
  Grid2D<float> old=state.terrain;
  //output(state.terrain);
  std::cout<<"BEFORE:"<<std::endl;
  //output(state);
  for(int i=0;i<4;i++) {
    sim.update(dt,true,true);
    std::cout<<"I0 "<<i<<std::endl;
  }
  for(int i=0;i<240;i++) {
    sim.update(dt,true,false);
    std::cout<<"I1 "<<i<<std::endl;
  }
  std::cout<<"AfTER RAIN:"<<std::endl;
  //output(state);
  for(int i=0;i<600;i++) {
    sim.update(dt,false,false);
    std::cout<<"I2 "<<i<<std::endl;
  }

  std::cout<<"AfTER:"<<std::endl;
  //output(state);

  //diff(old,state.terrain);
  //output(old);
  char key;
  float val=1;
  int frame=0;
  do {
    Generator::view(state,val,val+1,step,frame++);
    key=Generator::waitForKey();
    switch((int)key) {
      case 60:
        val-=1;break;
      case 62:
        val+=1;break;
    }
    sim.update(dt,false,false);
    std::cout<<"KEY:"<<(int)key<<std::endl;
  }while(key!=27);
  Generator::close();
  return 0;
}
