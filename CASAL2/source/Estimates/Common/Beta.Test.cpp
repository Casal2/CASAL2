/**
 * @file Beta.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 17/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "../../Estimates/Manager.h"
#include "../../Model/Models/Age.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/Models/TwoSexNoEstimates.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace estimates {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const string estimate_beta =
    R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type beta
mu 0.3
sigma 0.05
a 0
b 10
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Beta) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_beta, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1726.6295023194364, obj_function.score());

  Estimate* estimate = model_->managers()->estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 7.2724038680087295);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2476.5137933614251);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2367.250113991935);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2230.4867585646953);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -2066.4915312599851);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), -1868.5574163359895);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
