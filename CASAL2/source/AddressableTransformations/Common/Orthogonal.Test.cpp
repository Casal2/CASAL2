/**
 * @file Orthogonal.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/11
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "Orthogonal.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.Mock.h"
#include "../../Model/Objects.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/Models/TwoSex.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa::addressabletransformations {
using niwa::testfixtures::InternalEmptyModel;

// Test for positive x1 and x2
TEST(OrthogonalTransformationTest, PositiveX1X2) {
  double x1 = 4.0, x2 = 2.0, y1, y2, x1_rec, x2_rec;
  orthogonalTransform(x1, x2, y1, y2);
  inverseOrthogonalTransform(y1, y2, x1_rec, x2_rec);

  EXPECT_NEAR(x1, x1_rec, 1e-9);
  EXPECT_NEAR(x2, x2_rec, 1e-9);
}

// Test for positive x1, negative x2
TEST(OrthogonalTransformationTest, PositiveX1NegativeX2) {
  double x1 = 4.0, x2 = -2.0, y1, y2, x1_rec, x2_rec;
  orthogonalTransform(x1, x2, y1, y2);
  inverseOrthogonalTransform(y1, y2, x1_rec, x2_rec);

  EXPECT_NEAR(x1, x1_rec, 1e-9);
  EXPECT_NEAR(x2, x2_rec, 1e-9);
}

// Test for small positive x1, large negative x2
TEST(OrthogonalTransformationTest, SmallPositiveX1LargeNegativeX2) {
  double x1 = 0.1, x2 = -3.0, y1, y2, x1_rec, x2_rec;
  orthogonalTransform(x1, x2, y1, y2);
  inverseOrthogonalTransform(y1, y2, x1_rec, x2_rec);

  EXPECT_NEAR(x1, x1_rec, 1e-9);
  EXPECT_NEAR(x2, x2_rec, 1e-9);
}

TEST(OrthogonalTransformationTest, UsingSameValuesAsModelTest) {
  double x1 = 8, x2 = 3, y1, y2, x1_rec, x2_rec;
  orthogonalTransform(x1, x2, y1, y2);
  inverseOrthogonalTransform(y1, y2, x1_rec, x2_rec);

  EXPECT_NEAR(x1, x1_rec, 1e-9);
  EXPECT_NEAR(x2, x2_rec, 1e-9);
}

/**
 * Test the transformation in model
 */
TEST_F(InternalEmptyModel, AddressableTransformation_Orthogonal) {
  const string transform_for_twosex =
      R"(
        @parameter_transformation orth
        type orthogonal
        parameters selectivity[FishingSel].a50 selectivity[FishingSel].ato95
    )";

  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 27);
  AddConfigurationLine(transform_for_twosex, __FILE__, 31);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2698.1334330594045, obj_function.score(), 1e-4);

  auto product_parameter = model_->objects().GetAddressable("parameter_transformation[orth].product_parameter");
  EXPECT_NEAR(24, *product_parameter, 1e-2);

  auto quotient_parameter = model_->objects().GetAddressable("parameter_transformation[orth].quotient_parameter");
  EXPECT_NEAR(2.6666666666666665, *quotient_parameter, 1e-2);
}

}  // namespace niwa::addressabletransformations

#endif  // TESTMODE