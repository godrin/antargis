#include "ant_path_finder_complete.h"
#include "path.h"
#include "map.h"
#include "ant_hero.h"
#include "ag_fs.h"

AntPathFinderComplete::AntPathFinderComplete ( AntMap *pMap ) :map ( pMap )
{
  AGString filename=pMap->getFilename() +".path.tmp";
  if ( fileExists ( filename ) )
    {
      loadFrom ( filename );
    }
  else
    {
      computeEverything();
      saveTo ( filename );
    }
}

void AntPathFinderComplete::loadFrom ( const AGString& filename )
{
  AGString content=loadFile ( filename );
  BinaryStringIn stream ( content );
  SimpleGraph *land=new SimpleGraph ( stream );
  StoredHeuristicFunction *landHeuristic=new StoredHeuristicFunction ( stream );

  pathfinder[LAND]=new Pathfinder ( land,landHeuristic,0 );

  SimpleGraph *water=new SimpleGraph ( stream );
  StoredHeuristicFunction *waterHeuristic=new StoredHeuristicFunction ( stream );

  pathfinder[WATER]=new Pathfinder ( water,waterHeuristic,0 );
}
void AntPathFinderComplete::saveTo ( const AGString& filename )
{
  BinaryStringOut mstream;
  graphs[LAND]->printTo ( mstream );
  heuristic[LAND]->printTo ( mstream );
  graphs[WATER]->printTo ( mstream );
  heuristic[WATER]->printTo ( mstream );
  saveFile ( filename,mstream.getString() );
}



void AntPathFinderComplete::computeEverything()
{
  computeMode ( LAND );
  computeMode ( WATER );
}
MapPathWeighter *AntPathFinderComplete::getPathWeighter ( MODE mode )
{
  switch ( mode )
    {
    case LAND:
      return new MapPathWeighter ( map,false );
    case WATER:
      return new MapPathWeighter ( map,true );
    default:
      throw std::runtime_error("unknown pathfinder mode");
    }
  return 0;
}
void AntPathFinderComplete::computeMode ( AntPathFinderComplete::MODE mode )
{
  // build a map-height/distance weighter
  MapPathWeighter *weighter=getPathWeighter ( mode );

  // set initial distance of waypoints
  float minDist=4;

  SimpleGraph *sgraph=0;
  do
    {
      // make a path-finding graph

      sgraph=makeGraph ( map,weighter,minDist );
      minDist*=2;

    }
  while ( sgraph->size() >2000 ); // # use a smaller resolution, if there are too many nodes in graph

  assert ( sgraph->size() >0 );
  assert ( sgraph->edges() >0 );

  // copy to a decimating graph
  DecimatedGraph *graph=new DecimatedGraph ( *sgraph );

  // compute a decimation-factor
  float factor=1.0-800.0/graph->size();
  /*
      # debugging settings
      #factor=0.4
      #factor=1.0-220.0/@dgraph.size
  */
  graph->decimate ( factor,weighter );

  // compute a distance-field and use this as a pre-computed heuristic
  heuristic[mode]=computeHeuristic ( graph );
  graphs[mode]=graph;
  pathfinder[mode]=new Pathfinder ( graphs[mode],heuristic[mode],0 );

}

std::list< AGVector2> AntPathFinderComplete::computePath ( const AGVector2& from, const AGVector2& to, AntHero* hero )
{
  if ( pathfinder.size() ==0 )
    throw int();
  Pathfinder *f=pathfinder[getMode ( hero )];
  return f->computePath ( from,to );
}


AntPathFinderComplete::MODE AntPathFinderComplete::getMode ( AntHero* e )
{
  return e->resource.get ( "boat" ) >0?WATER:LAND;

}

std::list< AGVector2 > AntPathFinderComplete::refinePath ( std::list< AGVector2 > waypoints, AntHero* hero )
{
  MODE mode=getMode ( hero );
  return pathfinder[mode]->refinePath ( waypoints,getPathWeighter ( mode ) );

}
