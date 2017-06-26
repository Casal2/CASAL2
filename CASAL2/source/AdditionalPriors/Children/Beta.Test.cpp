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

#include "Model/Model.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockBeta : public Beta {
public:
  MockBeta(Model* model, double mu, double sigma, double a, double b, double* parameter) : Beta(model) {
    mu_ = mu;
    sigma_ = sigma;
    a_ = a;
    b_ = b;
    addressable_ = parameter;
  }
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, Beta) {
  // layout is mu, sigma, a, b, expected_score
  vector<vector<double>> values = {
      { 4, 2, 2, 7, 5, 1.3640928573264497},
      { 4, 2, 2, 7, 6, 1.1090354888959124}
  };

  Model model;
  for (auto line : values) {
    MockBeta beta(&model, line[0], line[1], line[2], line[3], &line[4]);
    EXPECT_DOUBLE_EQ(line[5], beta.GetScore());
  }
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
