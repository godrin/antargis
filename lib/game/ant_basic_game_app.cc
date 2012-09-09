#include "ant_basic_game_app.h"
#include "terrain.h"
#include "mesh.h"
#include "anim_mesh.h"
#include "entity.h"

AntBasicGameApp::AntBasicGameApp ( int w, int h ) : GLApp ( w, h )
{
  controlsDisabled=false;
}

void AntBasicGameApp::eventClick ( const PickResult& pNodes, int button )
{
  GLApp::eventClick ( pNodes, button );

  if ( controlsDisabled )
    {
      cdebug ( "CONTROLS DISABLED" );
      return ;
    }
  if ( pNodes.size() >0 )
    {
      PickNode first=pNodes.at ( 0 );
      if ( dynamic_cast<TerrainPiece*> ( first.node ) ||
           dynamic_cast<WaterPiece*> ( first.node ) )
        {
          eventMapClicked ( first.pos.dim2(),button );
        }
      else
        {
          eventEntitiesClicked ( pNodes,button );
        }
    }
}

void AntBasicGameApp::eventHover ( const PickResult& pNodes, int button )
{
  GLApp::eventHover ( pNodes, button );
  if ( pNodes.size() >0 )
    {
      PickNode n=pNodes[0];
      SceneNode *snode=n.node;
      bool ok=false;
      Mesh *mesh=dynamic_cast<Mesh*> ( snode );
      AnimMesh *animMesh=dynamic_cast<AnimMesh*> ( snode );
      if ( mesh||animMesh )
        {
          AntEntity *ent=getMap()->getEntity ( snode );
          if ( ent )
            {
              hoverEntity ( ent );
              ok=true;
            }
        }
      if ( !ok )
        {
          hoverEntity ( 0 );
        }
    }
}

void AntBasicGameApp::hoverEntity ( AntEntity* e )
{
  AntEntity *c=getMap()->getEntity ( hoveredEntityId );
  if ( c )
    {
      c->hovered ( false );
    }
  if ( e )
    {
      hoveredEntityId=e->getID();
      e->hovered ( true );
    }
  else
    hoveredEntityId=-1;
}
