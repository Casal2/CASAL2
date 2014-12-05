/**
 * @file Inverse.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Inverse.h"

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

const string estimate_transformations_inverse =
R"(
@estimate e1
parameter selectivity[FishingSel].a50
lower_bound 0
upper_bound 2.99
type beta
mu 0.3
sigma 0.05
a 0
b 10
transformation inverse
)";


/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Estimates_Transformations, Inverse) {
  estimates::TransformationPtr transform = estimates::TransformationPtr(new Inverse());

  EXPECT_DOUBLE_EQ(2.5, transform->Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform->Untransform(2.5));

  EXPECT_DOUBLE_EQ(0.008, transform->Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform->Untransform(0.008));

  EXPECT_DOUBLE_EQ(0.025, transform->Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform->Untransform(0.025));
}

/**
 *
 */
TEST_F(InternalEmptyModel, Estimates_Transformations_Inverse) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 33);
  AddConfigurationLine(estimate_transformations_inverse, __FILE__, 34);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kEstimation);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_NEAR(1726.6294938882004, obj_function.score(), 1e-7);

  EstimatePtr estimate = estimates::Manager::Instance().GetEstimate("selectivity[FishingSel].a50");
  if (!estimate)
    LOG_ERROR("!estimate");
  EXPECT_NEAR(estimate->value(), 7.2720802736802668, 1e-7);

  // Check results
  estimate->set_value(1.0);
  EXPECT_DOUBLE_EQ(1, estimate->GetTransformedValue());
  estimate->set_value(10.0);
  EXPECT_DOUBLE_EQ(0.10000000000000001, estimate->GetTransformedValue());
  estimate->set_value(100.0);
  EXPECT_DOUBLE_EQ(0.01, estimate->GetTransformedValue());
  estimate->set_value(4.0);
  EXPECT_DOUBLE_EQ(0.25, estimate->GetTransformedValue());
  estimate->set_value(5.0);
  EXPECT_DOUBLE_EQ(0.20000000000000001, estimate->GetTransformedValue());
}

}
}
}
#endif /* ifdef TESTMODE */
