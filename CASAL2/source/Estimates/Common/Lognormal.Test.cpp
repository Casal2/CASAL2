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

const string estimate_lognormal =
    R"(
@estimate
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type lognormal
mu 3
cv 5
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Lognormal) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_lognormal, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2563.1357030901372, obj_function.score());

  Estimate* estimate = model_->managers()->estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 8.9817107756936174);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.043178942974616268);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.92290644861305016);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.505874355920795);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.9500982814472527);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 2.3121580779587432);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
