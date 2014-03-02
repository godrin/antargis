#include <boost/test/unit_test.hpp>

#include "ant_hljob_fighting.h"
#include "ant_hero.h"

BOOST_AUTO_TEST_CASE(HlFightJob)
{
  auto myHero=new AntHero(0);
  auto targetHero=new AntHero(0);

  AntHlJobFighting *fightJob=new AntHlJobFighting(myHero,targetHero);
  BOOST_CHECK_EQUAL(2, 2);
}
