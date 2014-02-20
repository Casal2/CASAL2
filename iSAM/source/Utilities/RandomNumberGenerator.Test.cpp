/**
 *
 */
#ifdef TESTMODE

// headers
#include "RandomNumberGenerator.h"

#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// namespaces
namespace isam {
namespace utilities {

using std::cout;
using std::endl;


TEST(RandomNumberGenerator, Reset) {

  RandomNumberGenerator& rng = RandomNumberGenerator::Instance();

  rng.Reset(2468);

  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453,   rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428,  rng.uniform());

  rng.Reset(2468);

  EXPECT_DOUBLE_EQ(0.45965513936243951, rng.uniform());
  EXPECT_DOUBLE_EQ(0.13950349437072873,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.51783327478915453,   rng.uniform());
  EXPECT_DOUBLE_EQ(0.12138748820871115,  rng.uniform());
  EXPECT_DOUBLE_EQ(0.48201335011981428,  rng.uniform());
}


} /* namespace utilities */
} /* namespace isam */


#endif
