#define BOOST_TEST_MODULE SqrTests
#include <boost/test/unit_test.hpp>
#include "ant_hero.h"

BOOST_AUTO_TEST_CASE(FailTest)
{
      BOOST_CHECK_EQUAL(2, 2);
}

BOOST_AUTO_TEST_CASE(PassTest)
{
      BOOST_CHECK_EQUAL(4, 4);
}
