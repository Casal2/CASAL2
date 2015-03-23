/**
 * @file String.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "String.h"

// namespaces
namespace niwa {
namespace utilities {
namespace strings {

using ::testing::Return;

/**
 * Test the results of our selectivity are correct
 */
TEST(Utilities, String) {

  string test = "good_label";
  EXPECT_EQ("", find_invalid_characters(test));

  test = "bad!!label";
  EXPECT_EQ("!!", find_invalid_characters(test));

  test = "good[]()._-:";
  EXPECT_EQ("", find_invalid_characters(test));

  test = "bad%la^bel!";
  EXPECT_EQ("%^!", find_invalid_characters(test));
}


} /* namespace string */
} /* namespace utilities */
} /* namespace niwa */

#endif
