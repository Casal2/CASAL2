/**
 * @file Difference.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/11
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "Difference.h"

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

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, AddressableTransformation_Difference) {
  const string simple_difference_for_twosex =
      R"(
      @parameter_transformation trans_sel
      type difference
      parameters selectivity[FishingSel].a50 selectivity[FishingSel].ato95
      difference_parameter 5
    )";

  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 27);
  AddConfigurationLine(simple_difference_for_twosex, __FILE__, 31);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2698.1334330594045, obj_function.score(), 1e-4);

  auto difference_parameter = model_->objects().GetAddressable("parameter_transformation[trans_sel].difference_parameter");
  EXPECT_NEAR(5.0, *difference_parameter, 1e-2);
}

/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformation_Difference_Bad) {
  const string simple_difference_for_twosex_bad =
      R"(
        @parameter_transformation trans_sel
        type difference
        parameters selectivity[FishingSel].a50
      )";

  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 27);
  AddConfigurationLine(simple_difference_for_twosex_bad, __FILE__, 59);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

}  // namespace niwa::addressabletransformations
#endif  // TESTMODE