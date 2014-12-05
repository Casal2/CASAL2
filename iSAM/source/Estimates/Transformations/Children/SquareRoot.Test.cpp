/**
 * @file SquareRoot.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "SquareRoot.h"

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

const string estimate_transformations_squareroot =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 0
upper_bound 4.5
type beta
mu 0.3
sigma 0.05
a 0
b 10
transformation sqrt
)";

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Estimates_Transformations, SquareRoot) {
  estimates::TransformationPtr transform = estimates::TransformationPtr(new SquareRoot());

  EXPECT_DOUBLE_EQ(0.63245553203367588, transform->Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform->Untransform(0.63245553203367588));

  EXPECT_DOUBLE_EQ(11.180339887498949, transform->Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform->Untransform(11.180339887498949));

  EXPECT_DOUBLE_EQ(6.324555320336759, transform->Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform->Untransform(6.324555320336759));
}

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Transformations_SquareRoot) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_transformations_squareroot, __FILE__, 34);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(1726.6294925123473, obj_function.score());

  EstimatePtr estimate = estimates::Manager::Instance().GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_ERROR("!estimate");
  EXPECT_DOUBLE_EQ(7.2721721659220941, estimate->value());

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(1, estimate->GetTransformedValue());
  estimate->set_value(10.0);
  EXPECT_DOUBLE_EQ(3.1622776601683795, estimate->GetTransformedValue());
  estimate->set_value(100.0);
  EXPECT_DOUBLE_EQ(10, estimate->GetTransformedValue());
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(2.0, estimate->GetTransformedValue());
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(2.2360679774997898, estimate->GetTransformedValue());
}

}
}
}
#endif /* ifdef TESTMODE */
