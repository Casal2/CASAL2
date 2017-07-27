/**
 * @file Constant.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "Constant.h"

#include <gtest/gtest.h>

namespace niwa {

/**
 * Test the return override for the Constant selectivity is
 * working properly.
 */
TEST(Selectivities, Constant) {

  niwa::selectivities::Constant constant(nullptr);
  constant.parameters().Add(PARAM_LABEL, "unit_test_constant", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  constant.parameters().Add(PARAM_C, "83", __FILE__, __LINE__);
  constant.Validate();
  constant.Build();

  EXPECT_DOUBLE_EQ(83.0, constant.GetResult(0, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetResult(1, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetResult(2, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetResult(3, nullptr));
  EXPECT_DOUBLE_EQ(83.0, constant.GetResult(4, nullptr));
}

} /* namespace niwa */
#endif
