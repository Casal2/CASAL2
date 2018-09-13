/**
 * @file VectorSmoothing.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "VectorSmoothing.h"

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
class MockVectorSmoothing : public VectorSmoothing {
public:
	MockVectorSmoothing(Model* model, bool log_scale, unsigned lower_bound, unsigned upper_bound, unsigned r, double multiplier, vector<double>* parameter) : VectorSmoothing(model) {
		log_scale_ = log_scale;
		lower_ = lower_bound;
		upper_ = upper_bound;
    multiplier_ = multiplier;
    r_ = r;
    addressable_vector_ = parameter;
  }
};

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(AdditionalPriors, VectorSmoothing) {
  // layout is mu, sigma, a, b, expected_score
	vector<double> example_ycs = {0.478482, 0.640663, 0.640091, 0.762361, 0.560125, 0.651637, 0.764833, 0.645498, 0.678341};
	double multiplier = 150;
	bool log_scale = true;
	unsigned upper_bound = 9;
	unsigned lower_bound = 1;
	unsigned r = 2;

  Model model;
  MockVectorSmoothing vector_smooth_log(&model, log_scale, lower_bound, upper_bound, r, multiplier, &example_ycs);
  EXPECT_DOUBLE_EQ(107.71417585898443, vector_smooth_log.GetScore());

  // test log scale false
  log_scale = false;
  MockVectorSmoothing vector_smooth(&model, log_scale, lower_bound, upper_bound, r, multiplier, &example_ycs);
  EXPECT_DOUBLE_EQ(46.63045995209999, vector_smooth.GetScore());
  // test r = 1
  r = 1;
  lower_bound = 4;
  MockVectorSmoothing vector_smooth_r(&model, log_scale, lower_bound, upper_bound, r, multiplier, &example_ycs);
  EXPECT_DOUBLE_EQ(11.611002769499997, vector_smooth_r.GetScore());
}

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
