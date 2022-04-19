/**
 * @file Ratio.Test.cpp
 * @author C. Marsh
 * @github https://github.com//Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../../Model/Model.h"
#include "../../TestResources/MockClasses/Model.h"
#include "Ratio.h"

// Namespaces
namespace niwa {
namespace additionalpriors {

using ::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockRatio : public Ratio {
public:
  MockRatio(shared_ptr<Model> model, double mu, double cv, double* parameter, double* second_parameter) : Ratio(model) {
    mu_          = mu;
    cv_          = cv;
    addressable_ = parameter;
    second_addressable_ = second_parameter;
    sigma_ = sqrt(log(1.0+cv_*cv_));
  }
};
/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, Ratio) {
  // layout is mu, cv, param1, param2, expected_score
  vector<vector<double>> values = {{0.80153, 0.0924, 0.0991556, 0.140099, -1.46167}}; // Taken from a CASAL model
  shared_ptr<Model> model = shared_ptr<Model>(new Model());
  for (auto line : values) {
    MockRatio ratio(model, line[0], line[1], &line[2],  &line[3]);
    EXPECT_NEAR(line[4], ratio.GetScore(), 1e-4);
  }
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
