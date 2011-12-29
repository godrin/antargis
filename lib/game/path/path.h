#ifndef PATH_H
#define PATH_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "height_map.h"
#include "heuristic.h"

#include <ag_serial.h>
#include <ag_surface.h>

/**
   The purpose of this file is some path-finding-algorithm based upon "A-star". (A*)
   
   Because path-finding is a somewhat hard topic, we should try some
   hierarchical approach:
   * build some graph out of the height-map - weight come from height and terrain-type (swamp and deep sand vs. plain grass)
   * nodes that are near each other get eliminated (thrown together), if terrain connection is possible (and not too much longer)
   * (mayto TODO: several levels of this graph can be used, according to the distance that has to be gone)
   * for people that have boats, water can be crossed, so there has to be a different path-graph !

   Computation goes as follows:
   * You define a Path-Weighter, that weighs distances on a map (or without a map - for very simple cases (that we won't use here though))
   * Feed this into a makeGraph(...)-call
   * decimate the graph, if you want to
   * compute a heuristic on the resulting-graph - e.g. the full-computed distance-field for all nodes (in StoredHeuristicFunction)
   * create a Pathfinder-object using graph and heuristic
   * use this Pathfinder to compute distances and pathes :-)

   NOTE:
   * Within the game these classes won't be used directly, but through the CombinedPathFinder-ruby-class
*/

class Heuristic;

/**
   PathWeighter is a (not-really-used) base-class for the path-weighing-facility. It though stores, if water is
   passable.
   
   FIXME: integrate PathWeighter into MapPathWeighter!
 */
class AGEXPORT PathWeighter
{
 public:
  PathWeighter(bool pWaterPassable=false);
  virtual ~PathWeighter() throw();
  /// this function can be overridden - and will be
  virtual float weight(float h0,float h1);

  bool isWaterPassable() const;
 private:
  bool mWaterPassable;
};

class AGEXPORT MapPathWeighter:public PathWeighter
{
 public:
  MapPathWeighter(HeightMap *pMap,bool pWaterPassable);
  virtual float weight(const AGVector2 &a,const AGVector2 &b);
  virtual bool accessible(const AGVector2 &a);
 private:

  virtual float complexWeight(const AGVector2 &a,const AGVector2 &b);

  /// compute a weight for a flank. Compares the heights a and b. used by complexWeight
  virtual float weightHeight(float a,float b) const;

  HeightMap *mMap;
};


/**
   AGVector2Sort is a simple sorting class for use in sorting containers, like map or set.
 */
struct AGEXPORT AGVector2Sort
{
  public:
  bool operator()(const AGVector2 &a,const AGVector2 &b)
  {
    return a[0]<b[0] || (a[0]==b[0] && a[1]<b[1]);
  }
};


class AGEXPORT SimpleGraph
{
 public:
  struct Node;
  struct Edge;

  typedef std::list<Edge*> Edges;
  typedef std::list<Node*> Nodes;

#ifndef SWIG
  class NodePtrCompare
  {
  public:
    bool operator()(const Node *a,const Node *b);
  };
    
  class EdgePtrCompare
  {
  public:
    bool operator()(const Edge *a,const Edge *b);
  };

  // only used for processing and not for storing !
  struct HalfEdge
  {
    Node *a,*b;
    float w;
  };
  struct Edge
  {
    Node *a,*b;
    float w0,w1;

    Edge(Node *pa,Node *pb,float p0,float p1);
    ~Edge();

    bool operator<(const Edge &e) const;
    bool operator==(const Edge &e) const;
    float maxWeight() const;

    Node *getOther(Node *n);

  };

  struct Node
  {
    AGVector2 p;
    Edges edges;
    float tmpWeight;

    typedef std::map<Node*,float,NodePtrCompare> NodeMap;
    
    ~Node();

    NodeMap getNextNodes();

    bool hasEdge(Edge *e);

  };

  struct EdgeSort
  {
    public:
    bool operator()(const Edge*a,const Edge *b)
    {
      float w0=weight(a);
      float w1=weight(b);
      return w0<w1 || (w0==w1 && a<b);
    }
    private:
    float weight(const Edge *a)
    {
      return a->w0+a->w1;
    }
  };
#endif
  SimpleGraph();
  SimpleGraph(const SimpleGraph &g);
  SimpleGraph(BinaryIn &pIn);


  #ifndef SWIG
  Node *addNode(const AGVector2 &p);
  /**
     @param w0 is weight of going from a to b
     @param w1 is weight of goinf from b to a
  */
  Edge *addEdge(Node *a,Node *b,float w0,float w1);
#endif
  void removeEdge(Edge *e);
  void removeNode(Node *n);
  void print();
  void printInfo();

  ~SimpleGraph() throw();


  void check();
  

  Node *findNearest(const AGVector2 &p);
  AGVector2 findNearestVector(const AGVector2 &p);

  float width() const;

  /**
     @return count of nodes
  */
  size_t size() const;

  /**
     @return edge count
  */
  size_t edges() const;

  std::pair<AGVector2,AGVector2> getEdgePosition(size_t i);


  void paint(const AGRect2& r,AGPaintTarget &t,Heuristic &heuristic);
  void paintNode(const AGRect2& r,AGPaintTarget &t,const AGVector2 &p,const AGColor &c);

  void printTo(BinaryOut &os) const;

 protected:

  Node *findNode(const AGVector2 &p);

  // FIXME: quadtree out of nodes !!!

  typedef std::map<AGVector2,Node*,AGVector2Sort> NodeMap;
  typedef std::set<Node*,NodePtrCompare> NodeSet;
  typedef std::set<Edge*,EdgeSort> EdgeSet;
  NodeMap mNodeMap;
  NodeSet mNodes;
  EdgeSet mEdges;
  float mWidth;


  friend HeuristicFunction *computeHeuristic(SimpleGraph *g);

};

AGEXPORT SimpleGraph *makeGraph(HeightMap *pMap, MapPathWeighter *pWeighter,size_t res=1);
AGEXPORT HeuristicFunction *computeHeuristic(SimpleGraph *g);


class AGEXPORT DecimatedGraph:public SimpleGraph
{
 public:
  DecimatedGraph();
  DecimatedGraph(const SimpleGraph &g);
  ~DecimatedGraph() throw();
  void decimate(float amount,MapPathWeighter *pWeighter);
 private:
  void tryRemove(Edge *e,MapPathWeighter *pWeighter);
  void collapseEdge(Edge *e,MapPathWeighter *pWeighter);

  Edge makeEdge(Node *a,Node *b,MapPathWeighter *pWeighter);

};

struct Path;

class AGEXPORT Heuristic
{
  AGVector2 to;
  HeuristicFunction *p;
 public:
  Heuristic(const AGVector2 &pTo,HeuristicFunction *f):to(pTo),p(f)
    {
    }
  bool operator()(const Path &a,const Path &b);
  float distance(const AGVector2&from);
};

#ifndef  SWIG

struct AGEXPORT Path:public std::list<SimpleGraph::Node*>
{
  float weight;
  
  Path();
  
  float getWeight(Heuristic *h) const;
  
  void push(SimpleGraph::Node *n,float w);
  
  void paint(const AGRect2 &r,AGPaintTarget &t,float scale);
  
};
#endif

class AGEXPORT PathDebugging
{
 public:
  virtual ~PathDebugging()
    {
    }
  virtual void debugPath(Path &p,float heuristic)
  {
  }
};
  

class AGEXPORT Pathfinder
{
 public:

  Pathfinder(SimpleGraph *pGraph,HeuristicFunction *pHeuristic,PathDebugging *pDebugger=0);


  std::list<AGVector2> computePath(const AGVector2 &from, const AGVector2 &to);

  std::list<AGVector2> refinePath(const std::list<AGVector2> &p,MapPathWeighter *pWeighter);
  
 private:
  SimpleGraph *mGraph;
  HeuristicFunction *mHeuristic;
  PathDebugging *mDebug;
};

#endif
