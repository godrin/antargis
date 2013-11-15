#include "generator_viewer.h"
#include "simulation_state.h"

#include <ncurses.h>

namespace Generator {

  bool inited=false;

  void init() {
    if(inited)
      return;

    std::cout<<"INIT SCREEN"<<std::endl;

    initscr();
    if (nodelay (stdscr, 1) == ERR) {
      // some error occurred.
    } 

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // sand above water
    init_pair(2, COLOR_YELLOW, COLOR_BLUE);  // sand under water
    init_pair(3, COLOR_YELLOW, COLOR_YELLOW);  // deep sand
    init_pair(4, COLOR_WHITE, COLOR_BLACK); // air
    init_pair(5, COLOR_WHITE, COLOR_BLUE); // water

    inited=true;
  }

  void close() {
    endwin();
    inited=false;
  }

  char waitForKey() {
    return getch();
  }

  void view(Map2D &map) {
    move(10,10);
    attron(COLOR_PAIR(1));
    addch('X');
    attroff(COLOR_PAIR(1));

    getch();
    endwin();
  }

  void view(const Grid2D<float> &grid,float from,float to,int step) {
    init();
    for(int y=0;y<grid.height()/step;y++) {
      for(int x=0;x<grid.width()/step;x++) {
        move(y,x*2);
        float val=grid(y*step,x*step);
        if(val<from)
          addch('X');
        else if(val<to)
          addch('.');
        else
          addch(' ');
      }
    }
  }
  void view(const SimulationState &state,float from,float to,int step,int frame) {
    int w=state.terrain.width();
    int h=state.terrain.height();
    init();

    for(int y=0;y<h/step;y++) {
      for(int x=0;x<w/step;x++) {
        float t=state.terrain(y*step,x*step);
        float w=state.water(y*step,x*step);
        float s=state.suspendedSediment(y*step,x*step);
        float r=state.rock(y*step,x*step);
        r=0;
        s=0;

        move(y,x*2);

        if(r>to && false) {
          color_set(4,0);
          addch('#');
        } else if(r+t>to) {
          // deep sand
          if(state.rock(y*step,x*step)>0)
            color_set(4,0);
          else
            color_set(3,0);
          addch('#');
        } else  if(to<r+t+s) {
          // sediment
          color_set(1,0);
          addch('#');
        } else if(to<r+t+s+w) {
          color_set(5,0);
          addch('=');
        } else {
          color_set(1,0);
          addch('.');
        }
      }
    }
    color_set(4,0);
    mvprintw(0,0,"%d",frame);
    mvprintw(1,0,"level:%d   ",(int)to);
  }

}
