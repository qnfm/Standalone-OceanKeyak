#include <gtest/gtest.h>


extern "C" {
//#include <assert.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "config.h"
#include "Keyakv2.h"
#include "testKeyakv2.h"
}




// Demonstrate some basic assertions.
TEST(KeyakTest, OceanKeyakAssertions)
{
  std::cout << "* Ocean Keyak: "
            << "256-bit SIMD implementation ( AVX2, 12 rounds unrolled )" << std::endl;
  // testMotorist();
  EXPECT_EQ(testKeyak(), 0);
  // for (int testmode = 0; testmode <= 5; ++testmode)
  // {
  //   testKeyakOneMode(testmode, file, expected);
  // }
  // return (0);
  // // Expect two strings not to be equal.
  // EXPECT_STRNE("hello", "world");
  // // Expect equality.
  // EXPECT_EQ(7 * 6, 42);
}



