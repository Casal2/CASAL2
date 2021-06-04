/**
 * @file ElementDifference.Test.cpp
 * @author C.Marsh
 * @github https://github.com/niwa
 * @date 13/7/17
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../../Model/Model.h"
#include "../../TestResources/MockClasses/Model.h"
#include "ElementDifference.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using ::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockElementDifference : public ElementDifference {
public:
  MockElementDifference(shared_ptr<Model> model, double multipler, double* second_parameter, double* parameter) : ElementDifference(model) {
    multiplier_         = multipler;
    addressable_        = parameter;
    second_addressable_ = second_parameter;
  }
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, ElementDifference) {
  // layout is mu, sigma, a, b, expected_score
  vector<vector<double>> values = {{1000, 0.32, 0.56, 57.600000000000023}, {100, 3.4, 7.44, 1632.1600000000008}};

  shared_ptr<Model> model = shared_ptr<Model>(new Model());
  for (auto line : values) {
    MockElementDifference element_difference(model, line[0], &line[1], &line[2]);
    EXPECT_DOUBLE_EQ(line[3], element_difference.GetScore());
  }
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
