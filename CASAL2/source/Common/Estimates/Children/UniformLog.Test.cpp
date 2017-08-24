/**
 * @file UniformLog.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Estimates/Manager.h"
#include "Common/Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexNoEstimates.h"

// Namespaces
namespace niwa {
namespace estimates {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const string estimate_uniform_log =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type uniform_log
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Uniform_Log) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_uniform_log, __FILE__, 33);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(2561.9956004278815, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 8.9820599333781583);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.0);
  estimate->set_value(2.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 0.69314718055994529);
  estimate->set_value(3.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.0986122886681098);
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.3862943611198906);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetScore(), 1.6094379124341003);
}

} /* namespace estimates */
} /* namespace niwa */
#endif
