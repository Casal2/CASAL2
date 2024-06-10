/**
 * @file LogNormal.Test.cpp
 * @author C. Marsh
 * @github https://github.com//niwa
 * @date 13/7/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../../Model/Model.h"
#include "../../TestResources/MockClasses/Model.h"
#include "LogNormal.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using ::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockLogNormal : public LogNormal {
public:
  MockLogNormal(shared_ptr<Model> model, double mu, double cv, double* parameter) : LogNormal(model) {
    mu_          = mu;
    cv_          = cv;
    addressable_ = parameter;
  }
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, LogNormal) {
  // layout is mu, sigma, a, b, expected_score
  vector<vector<double>> values = {{4.3, 0.05, 5.46, 13.239745898323584}, {2.3, 0.4, 6.24, 5.7044023501649734}};

  shared_ptr<Model> model = shared_ptr<Model>(new Model());
  for (auto line : values) {
    MockLogNormal log_normal(model, line[0], line[1], &line[2]);
    EXPECT_DOUBLE_EQ(line[3], log_normal.GetScore());
  }
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
