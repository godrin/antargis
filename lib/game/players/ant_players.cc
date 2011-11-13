#include "ant_players.h"
#include "ag_xml.h"
#include "ant_human_player.h"

AntPlayer* createPlayer(const Node& node, AntMap* pMap)
{
    AGString type=node.getName();
    AGString name=node.get("name");
    AntPlayer *player=0;
    if (type=="humanPlayer") {
        player=new AntHumanPlayer(pMap,name);
        player->loadXML(node);
    } else {
      cdebug("UNKNOWN:"<<type);
    }
    return player;
}
