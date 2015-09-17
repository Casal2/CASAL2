/**
 * @file Beta.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 15/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "Beta.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using ::testing::Return;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, Beta) {
  Beta beta;
  beta.parameters().Add(PARAM_LABEL, "beta", __FILE__, __LINE__);
  beta.parameters().Add(PARAM_TYPE,  PARAM_BETA, __FILE__, __LINE__);
  beta.parameters().Add(PARAM_METHOD, PARAM_RATIO, __FILE__, __LINE__);
  beta.parameters().Add(PARAM_MU, "4", __FILE__, __LINE__);
  beta.parameters().Add(PARAM_SIGMA, "2", __FILE__, __LINE__);
  beta.parameters().Add(PARAM_A, "2", __FILE__, __LINE__);
  beta.parameters().Add(PARAM_B, "7", __FILE__, __LINE__);

  beta.Validate();
  beta.Build();

//  EXPECT_DOUBLE_EQ(0.0, beta.score());
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
