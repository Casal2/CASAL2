/**
 * @file Lognormal.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexNoEstimates.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const string estimate_normal_by_stdev =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type normal_by_stdev
mu 1
sigma 2
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Normal_By_Stdev) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_normal_by_stdev, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2567.7578345277416, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 8.9750026989362155);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.125);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.5);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.125);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 2.0);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
