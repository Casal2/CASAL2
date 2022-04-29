/**
 * @file Basic.Test.cpp
 * @author  C.Marsh 
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../TestResources/MockClasses/Managers.h"
#include "../../TestResources/MockClasses/Model.h"
#include "Basic.h"

// Namespaces
namespace niwa {
namespace growthincrements {
using ::testing::Return;
using ::testing::ReturnRef;

class MockBasic : public Basic {
public:
  MockBasic(shared_ptr<Model> model, Double g_a, Double g_b, Double l_a, Double l_b) :
      Basic(model) {
      g_a_                    = g_a;
      g_b_                    = g_b;
      l_a_                  = l_a;
      l_b_                  = l_b;
  }
  Double Mockget_value(double length, double time_step_proportion) { return this->get_mean_increment(length, time_step_proportion); }
  };


/**
 * Test the results of our Schnute are correct
#' basic_growth
#' @param length
#' @param ga
#' @param gb
#' @param la
#' @param lb
#' @return mean length increment
#' 
basic_growth = function(length, ga, gb, la, lb) {
  ga + (gb - ga)*(length - la)/(lb - la);
}
length_bins = 1:43
paste(length_bins, collapse = ", ")
la = 20     
lb = 40
ga = 10    
gb = 1
result = vector()
for(i in 1:length(length_bins))
  result[i] = basic_growth(length_bins[i], ga, gb, la, lb)
paste(result, collapse = ", ")
 */
TEST(GrowthIncrements, Basic) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  vector<double> lengths = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};
  vector<double> result = {18.55, 18.1, 17.65, 17.2, 16.75, 16.3, 15.85, 15.4, 14.95, 14.5, 14.05, 13.6, 13.15, 12.7, 12.25, 11.8, 11.35, 10.9, 10.45, 10, 9.55, 9.1, 8.65, 8.2, 7.75, 7.3, 6.85, 6.4, 5.95, 5.5, 5.05, 4.6, 4.15, 3.7, 3.25, 2.8, 2.35, 1.9, 1.45, 1, 0.550000000000001, 0.0999999999999996, -0.35};
  MockBasic basic(model, 10.0, 1.0,  20.0, 40.0);
  for(unsigned i = 0; i < lengths.size(); i++) {
    EXPECT_NEAR(basic.Mockget_value(lengths[i], 1.0), result[i], 1e-3) << " i " << i << " get _value = " << basic.Mockget_value(lengths[i], 1.0) << " result " << result[i];
  } 
}


} /* namespace growthincrements */
} /* namespace niwa */

#endif
