/**
 * @file Log.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Log.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexNoEstimates.h"

// Namespaces
namespace niwa {
namespace estimates {
namespace transformations {

using niwa::testfixtures::InternalEmptyModel;

const string estimate_transformations_log =
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
transformation log
)";

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Estimates_Transformations, Log) {
  Log transform;

  EXPECT_DOUBLE_EQ(-0.916290731874155, transform.Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform.Untransform(-0.916290731874155));

  EXPECT_DOUBLE_EQ(4.8283137373023015, transform.Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform.Untransform(4.8283137373023015));

  EXPECT_DOUBLE_EQ(3.6888794541139363, transform.Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform.Untransform(3.6888794541139363));
}


/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Transformations_Log) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_transformations_log, __FILE__, 34);
  LoadConfiguration();

  model_->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(1726.6294925126288, obj_function.score());

  Estimate* estimate = model_->managers().estimate()->GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_FATAL() << "!estimate";
  EXPECT_DOUBLE_EQ(estimate->value(), 7.2721723781667542);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(0, estimate->GetTransformedValue());
  estimate->set_value(10.0);
  EXPECT_DOUBLE_EQ(2.3025850929940459, estimate->GetTransformedValue());
  estimate->set_value(100.0);
  EXPECT_DOUBLE_EQ(4.6051701859880918, estimate->GetTransformedValue());
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(estimate->GetTransformedValue(), 1.3862943611198906);
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(estimate->GetTransformedValue(), 1.6094379124341003);
}

}
}
}
#endif /* ifdef TESTMODE */
