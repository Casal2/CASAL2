/**
 * @file Logistic.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/11
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

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
#include "Difference.h"

namespace niwa::addressabletransformations {
using niwa::testfixtures::InternalEmptyModel;

/*
 * Sample unit test
 */
TEST_F(InternalEmptyModel, AddressableTransformation_Logistic) {
  const string transform_for_twosex =
      R"(
        @parameter_transformation loga50
        type logistic
        parameters selectivity[FishingSel].a50
        lower_bound 1
        upper_bound 10
    )";

  AddConfigurationLine(testresources::models::two_sex, "TestResources/Models/TwoSex.h", 27);
  AddConfigurationLine(transform_for_twosex, __FILE__, 31);
  LoadConfiguration();
  MockObjects mock_objects(model_);
  model_->Start(RunMode::kBasic);
  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(2698.1334330594045, obj_function.score(), 1e-4);

  auto log_total_parameter = model_->objects().GetAddressable("parameter_transformation[loga50].logistic_parameter");
  EXPECT_NEAR(1.2527629684953681, *log_total_parameter, 1e-2);
}

}  // namespace niwa::addressabletransformations
#endif  // TESTMODE