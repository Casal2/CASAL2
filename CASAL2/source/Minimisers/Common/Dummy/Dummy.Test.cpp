/**
 * @file Dummy.Test.cpp
 * @brief Tests for the placeholder minimiser used for types from another library
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifdef TESTMODE

#include "Dummy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../../Minimisers/Manager.h"
#include "../../../Model/Managers.h"
#include "../../../Model/Model.h"
#include "../../../TestResources/TestCases/TwoSexModel.h"
#include "../../../TestResources/TestFixtures/InternalEmptyModel.h"

namespace niwa {
using niwa::testfixtures::InternalEmptyModel;

/**
 * In the test library an adolc minimiser is a Dummy. An inactive one must not count
 * as active alongside the TwoSex model's active numerical_differences minimiser.
 */
TEST_F(InternalEmptyModel, Minimiser_Dummy_HonoursActiveFalse) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);
  string definitions = R"(
    @minimiser inactive_placeholder
    type adolc
    active false
    tolerance 1e-6
  )";
  AddConfigurationLine(definitions, __FILE__, 30);
  LoadConfiguration();

  model_->Start(RunMode::kTesting);

  auto minimiser = model_->managers()->minimiser()->active_minimiser();
  ASSERT_NE(minimiser, nullptr);
  EXPECT_EQ("gammadiff", minimiser->label());
}

}  // namespace niwa
#endif  // TESTMODE
