/**
 * @file LogNormal.Test.cpp
 * @author C. Marsh
 * @github https://github.com//niwa
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
#include <vector>

#include "../../Model/Model.h"
#include "../../TestResources/MockClasses/Model.h"
#include "UniformLog.h"

// Namespaces
namespace niwa::additionalpriors {

using ::testing::Return;

/*
 * Mock class to make testing easier
 */
class MockUniformLog : public UniformLog {
public:
  MockUniformLog(shared_ptr<Model> model, double* target) : UniformLog(model) { addressable_ = target; }
};

/**
 * Test this additional prior
 */
TEST(AdditionalPriors, UniformLog) {
  shared_ptr<Model> model  = shared_ptr<Model>(new Model());
  double            target = 100;

  MockUniformLog ul(model, &target);

  // Set up some pairs of n, log(n) to be checked by our uniform log prior
  std::vector<std::pair<double, double>> values
      = {{5, 1.609438}, {10, 2.302585}, {15, 2.70805}, {20, 2.995732}, {25, 3.218876}, {22, 3.091042}, {12, 2.484907}, {2, 0.69314718055994529}};

  // iterate over the values and compare them against what we calculate
  for (auto pair : values) {
    target = pair.first;
    EXPECT_NEAR(pair.second, ul.GetScore(), 1e-5);
  }
}

} /* namespace niwa::additionalpriors */

#endif /* ifdef TESTMODE */
