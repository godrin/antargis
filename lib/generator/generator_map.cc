#include "generator_map.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>

namespace Generator {

  Map::Map(std::size_t w,std::size_t h,std::size_t d) {
    cells=new Cell[w*h*d];
    mW=w;
    mH=h;
    mD=d;
  }

  Map::~Map() {
    delete cells;
  }
  const Cell * const Map::get(std::size_t x,std::size_t y,std::size_t z) const {
    assert(x<mW);
    assert(y<mH);
    assert(z<mD);
    return get(x+y*mW+z*mW*mH);
  }
  Cell *Map::get(std::size_t x,std::size_t y,std::size_t z) {
    assert(x<mW);
    assert(y<mH);
    assert(z<mD);
    return get(x+y*mW+z*mW*mH);
  }
  const Cell * const Map::get(std::size_t i) const {
    return &(cells[i]);
  }
  Cell *Map::get(std::size_t i) {
    return &(cells[i]);
  }
  std::size_t Map::size() const {
    return mW*mH*mD;
  }

  Map2D::Map2D(std::size_t w,std::size_t h) {
    cells=new Cell[w*h];
    mW=w;
    mH=h;
  }

  Map2D::~Map2D() {
    delete cells;
  }
  const Cell * const Map2D::get(std::size_t x,std::size_t y) const {
    assert(x<mW);
    assert(y<mH);
    return get(x+y*mW);
  }
  Cell *Map2D::get(std::size_t x,std::size_t y) {
    assert(x<mW);
    assert(y<mH);
    return get(x+y*mW);
  }
  const Cell * const Map2D::get(std::size_t i) const {
    return &(cells[i]);
  }
  Cell *Map2D::get(std::size_t i) {
    return &(cells[i]);
  }
  std::size_t Map2D::size() const {
    return mW*mH;
  }

  std::size_t Map2D::getW() const {
    return mW;
  }
  std::size_t Map2D::getH() const {
    return mH;
  }

  void forAll(Map2D &map,boost::function<void (Cell*)> callback) {
    for(std::size_t i=0;i<map.size();i++) {
      callback(map.get(i));
    }
  }
  void forAll(Map &map,boost::function<void (Cell*)> callback) {
    for(std::size_t i=0;i<map.size();i++) {
      callback(map.get(i));
    }
  }


  void nullify(Cell *cell) {
    cell->data[0]=
      cell->data[1]=
      cell->data[2]=0;
  }


  void blur(Map2D &map,Cell::Type type) {
    for(size_t x=0;x<map.getW();x++) {
      for(size_t y=0;y<map.getH();y++) {

        int val=map.get(x,y)->data[type]*4;
        int count=4;

        if(x>0) {
          val+=map.get(x-1,y)->data[type];
          count++;
        }
        if(y>0) {
          val+=map.get(x,y-1)->data[type];
          count++;
        }
        if(x<map.getW()-1) {
          val+=map.get(x+1,y)->data[type];
          count++;
        }
        if(y<map.getH()-1) {
          val+=map.get(x,y+1)->data[type];
          count++;
        }

        map.get(x,y)->data[type]=val/count;
      }
    }
  }

  void interpolate(Map2D &from,Map2D &to, Cell::Type type) {
    float fx,fy,dx,dy;
    float a,b,c,d;
    size_t tx,ty,tx2,ty2;

    for(std::size_t y=0;y<to.getH();y++) {
      for(std::size_t x=0;x<to.getW();x++) {
        fx=float(x)*(from.getW()-1)/(to.getW()-1);
        fy=float(y)*(from.getH()-1)/(to.getH()-1);
        if(fx>from.getW()-1)
          fx=from.getW()-1;
        if(fy>from.getH()-1)
          fy=from.getH()-1;
        tx=fx;
        ty=fy;
        dx=fx-tx;
        dy=fy-ty;

        tx2=tx+1;
        ty2=ty+1;
        if(tx2>=from.getW())
          tx2=from.getW()-1;
        if(ty2>=from.getH())
          ty2=from.getH()-1;

        a=from.get(tx,ty)->data[type];    
        b=from.get(tx2,ty)->data[type];    
        c=from.get(tx,ty2)->data[type];    
        d=from.get(tx2,ty2)->data[type];    

        to.get(x,y)->data[type]=
          (1-dy)*((1-dx)*a+dx*b)+
          dy*((1-dx)*c+dx*d);

      }
    }
  }

  void add(Map2D &to,const Map2D &from,Cell::Type type) {
    assert(to.getW()==from.getW());
    assert(to.getH()==from.getH());
    for(int x=0;x<from.getW();x++) {
      for(int y=0;y<from.getH();y++) {
        to.get(x,y)->data[type]+=from.get(x,y)->data[type];
      }
    }
  }

  void output(Map2D &map,Cell::Type type) {
    for(size_t y=0;y<map.getH();y++) {
      for(size_t x=0;x<map.getW();x++) {
        printf("%4d ",(int)map.get(x,y)->data[type]);
      }
      std::cout<<std::endl;
    }
  }



}

