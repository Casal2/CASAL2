/**
 * @file NormalLog.Test.cpp
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

const string estimate_normal_log =
    R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type normal_log
mu 1
sigma 2
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Normal_Log) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_normal_log, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2562.1741691001221, obj_function.score());

  Estimate* estimate = model_->managers()->estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 8.9819352355739603);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.125);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.70491701215973412);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.099827836602655);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.4049472777990186);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.6558647335731045);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
