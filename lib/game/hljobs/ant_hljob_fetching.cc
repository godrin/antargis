#include "ant_hljob_fetching.h"
#include "entity.h"
#include <ant_man.h>
#include <ant_hero.h>

AntHLJobFetching::AntHLJobFetching(AntBoss* pBoss): AntHLJob(pBoss)
{

}
AGVector2 AntHLJobFetching::basePos()
{
    return getBossEntity()->getPos2D();
}
void AntHLJobFetching::check(AntMan* man)
{
    man->newRestJob(10);
}
void AntHLJobFetching::check(AntHero* hero)
{
    hero->newRestJob(10);
}
