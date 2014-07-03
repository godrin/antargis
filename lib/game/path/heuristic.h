#ifndef HEURISTIC_H
#define HEURISTIC_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_geometry.h>

#include <map>

class AGEXPORT HeuristicFunction
{
 public:
  typedef std::pair<AGVector2,AGVector2> Input;
  typedef float Output;

  virtual ~HeuristicFunction()
    {
    }

  virtual void printTo(BinaryOut &pOut);

  virtual Output operator()(const Input &input);

  float get(const AGVector2 &a,const AGVector2 &b);
};

#define NEW_STORE

class AGEXPORT StoredHeuristicFunction:public HeuristicFunction
{
 public:
  StoredHeuristicFunction();
  StoredHeuristicFunction(BinaryIn &pIn);

  void store(Input in,Output out);

  void store(const AGVector2 &from,const AGVector2 &to,float value);

  //  void display();

  virtual Output operator()(const Input &input);

  void printTo(BinaryOut &pOut);

 private:

  std::map<Input,Output> mMap;
#ifdef NEW_STORE
  std::map<AGVector2,size_t> mVecs;
  std::vector<float> mMapVec;

  size_t getIndex(const Input &input);
#endif

  
};

#endif

