/**
 * @file Uniform.Test.cpp
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

const string estimate_uniform =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type uniform
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Uniform) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_uniform, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2559.8003508774896, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 8.9824772794380507);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
