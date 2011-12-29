#ifndef __ANT_PATH_FINDER_COMPLETE_H
#define __ANT_PATH_FINDER_COMPLETE_H

#include <ag_geometry.h>

#include <list>
#include <map>

class DecimatedGraph;
class AntMap;
class Pathfinder;
class HeuristicFunction;
class AntHero;
class MapPathWeighter;

class AntPathFinderComplete  {
public:
    enum MODE {LAND,WATER};

    AntPathFinderComplete(AntMap *pMap);

    std::list<AGVector2> computePath(const AGVector2 &from,const AGVector2 &to,AntHero *hero);
    std::list<AGVector2> refinePath(std::list<AGVector2> waypoints,AntHero *hero);
private:
    void computeEverything();
    void computeMode(MODE mode);
    MapPathWeighter *getPathWeighter(MODE mode);
    MODE getMode(AntHero*e);

    AntMap *map;

    std::map<MODE,Pathfinder*> pathfinder;
    std::map<MODE,HeuristicFunction*> heuristic;
    std::map<MODE,DecimatedGraph*> graphs;

};

#endif
