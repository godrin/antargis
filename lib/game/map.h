/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * map.h
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef __MAP_H__
#define __MAP_H__

#include "height_map.h"
#include "heuristic.h"

#include <memory>

/**
 * \defgroup GameEngine BoA Game-Engine
 * \brief the C++ parts of the BoA game-engine
 *
*/

template<class T>
class QuadTree;

class AnimMesh;
class MeshBase;
class AntPlayer;
class AntHumanPlayer;
class Pathfinder;
class AntPathFinderComplete;
class AntBoss;
class SceneNode;

class AntEntitySelector
{
public:
  virtual bool ok ( AntEntity*e ) =0;
};

class AGEXPORT AntMap:public HeightMap
{
public:
  typedef std::list<AntEntity*> EntityList;
  typedef std::vector<AntEntity*> EntityVector;

  AntMap ( Scene *pScene,int w,int h );
  ~AntMap() throw();

  virtual void insertEntity ( AntEntity *e );
  virtual void removeEntity ( AntEntity *p );
  void clear() throw();

  int getNewID();
  void useID ( int id );

  void newMap ( int w,int h );

  EntityList getEntities ( const AGRect2&r );
  EntityList getSelectedEntities ( std::auto_ptr<AntEntitySelector> selector );
  EntityList getAllEntities();
  std::vector<AntEntity*> getEntities ( const AGString &pName );

  AntEntity *getEntity ( const SceneNode *pMesh );

  AntEntity *getEntity ( int id ) const;
  AntBoss *getBoss ( int id ) const;
  AntEntity *getByName ( const AGString &pName );

  AntEntity *getNext ( AntEntity *me,const AGString &pType,size_t atLeast=0 );
  EntityVector getNextList ( AntEntity *me,const AGString &pType,size_t atLeast=0 );

  void setHeuristic ( HeuristicFunction *pFunction );

  virtual void processXMLNode ( const Node &node );

  void saveXML ( Node &node ) const;
  bool loadXML ( const Node &node );

  void saveMap ( const AGString &pFilename );
  virtual bool loadMap ( const AGString &pFilename );
  virtual bool loadMapFromMemory ( const AGString &pMemory );

  /// advance every entity a single step of delta pTime
  void move ( float pTime );

  AGVector3 getPos ( const AGVector2 &pPos ) const;

  /// notify that (height)map was changed, reposition entities
  virtual void mapChanged();

  AntPlayer *getMyPlayer();
  AntPathFinderComplete *getCompletePathFinder();
  void setCompletePathFinder ( AntPathFinderComplete *finder );
  AGString getFilename() const;

private:
  void insertPlayer ( AntPlayer*player );
  void removePlayer ( AntPlayer *player );
  void setFilename ( const AGString &pFilename );


  typedef std::map<size_t,AntEntity*> EntityMap;

  EntityList mEntities;
  EntityMap mEntityMap;
  QuadTree<AntEntity> *mEntQuad;

  /// a list of entities to be removed in the next frame
  /// they're not deleted at once, because they may be used currently
  EntityList mToDel;

  typedef std::list<AntPlayer*> PlayerList;
  PlayerList mPlayers;
  AntHumanPlayer *myPlayer;

  int maxID;

  HeuristicFunction *mHeuristicFunction;
  AntPathFinderComplete *completePathFinder;
  AGString mFilename;

};


#endif
