#ifndef __ANT_HUMAN_PLAYER_H
#define __ANT_HUMAN_PLAYER_H

#include "ant_player.h"

class AntHumanPlayer:public AntPlayer {
public:
    AntHumanPlayer(AntMap* pMap, const AGString& pName);
    AGString xmlName();
    void assignJob(AntBoss *boss);
};

#endif
