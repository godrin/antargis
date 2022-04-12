#ifndef __GENERATOR_MAP_H
#define __GENERATOR_MAP_H

#include <boost/function.hpp>
#include <stdlib.h>
#include <vector>

namespace Generator {
struct Cell {
public:
  enum Type { WATER, SAND, ROCK };
  std::size_t data[3];
  /* std::size_t water;
   std::size_t sand;
   std::size_t rock;
   */
};

class Map2D {
private:
  Cell *cells;
  std::size_t mW, mH;

public:
  Map2D(std::size_t w, std::size_t h);
  ~Map2D();
  const Cell *const get(std::size_t x, std::size_t y) const;
  Cell *get(std::size_t x, std::size_t y);
  const Cell *const get(std::size_t i) const;
  Cell *get(std::size_t i);
  std::size_t size() const;
  std::size_t getW() const;
  std::size_t getH() const;
};

class Map {
private:
  Cell *cells;
  std::size_t mW, mH, mD;

public:
  Map(std::size_t w, std::size_t h, std::size_t d);
  ~Map();
  const Cell *const get(std::size_t x, std::size_t y, std::size_t z) const;
  Cell *get(std::size_t x, std::size_t y, std::size_t z);
  const Cell *const get(std::size_t i) const;
  Cell *get(std::size_t i);
  std::size_t size() const;
};

void nullify(Cell *cell);
void forAll(Map &map, boost::function<void(Cell *)> callback);
void forAll(Map2D &map, boost::function<void(Cell *)> callback);
void blur(Map2D &map, Cell::Type type);
void interpolate(Map2D &from, Map2D &to, Cell::Type type);
void add(Map2D &to, const Map2D &from, Cell::Type type);

void output(Map2D &map, Cell::Type type);

class random {
  std::size_t amount;
  Cell::Type type;

public:
  random(Cell::Type t, std::size_t h) {
    amount = h;
    type = t;
  }
  void operator()(Cell *c) { c->data[type] = rand() % amount; }
};

}; // namespace Generator

#endif
