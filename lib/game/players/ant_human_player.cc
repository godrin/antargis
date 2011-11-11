#include "ant_human_player.h"
AntHumanPlayer::AntHumanPlayer(AntMap* pMap, const AGString& pName): AntPlayer(pMap, pName)
{

}
void AntHumanPlayer::assignJob(AntBoss* boss)
{

}
AGString AntHumanPlayer::xmlName()
{
    return "humanPlayer";
}
