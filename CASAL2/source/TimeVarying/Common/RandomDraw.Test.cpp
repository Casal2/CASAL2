/**
 * @file RandomDraw.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/04/16
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "RandomDraw.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Report.h"
#include "Reports/Test/AddressableValue.h"
#include "TestResources/Models/TwoSex.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeVarying/Manager.h"

namespace niwa::timevarying {
using niwa::testfixtures::InternalEmptyModel;

TEST_F(InternalEmptyModel, Timevarying_RandomDraw) {
  const string extra_config = R"(
    @time_varying random_draw
    type random_draw
    parameter selectivity[FishingSel].a50
    years  1994 1995 1996 1997 1998 1999 2000
    mean 5.0
    sigma 1.0
    lower_bound 1
    upper_bound 8
    distribution normal
  
    @report test_report
    type addressable_value
    time_step step_two
    addressable selectivity[FishingSel].a50
    )";

  AddConfigurationLine(niwa::testresources::models::two_sex, "TestResources/Models/TwoSex.h", 31);
  AddConfigurationLine(extra_config, __FILE__, 30);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  auto* time_varying = model_->managers()->time_varying()->get("random_draw");
  ASSERT_NE(nullptr, time_varying);

  RandomDraw* random_draw = dynamic_cast<RandomDraw*>(time_varying);
  ASSERT_NE(nullptr, random_draw);

  Report* report = model_->managers()->report()->get("test_report");
  ASSERT_NE(nullptr, report);

  reports::test::AddressableValue* avreport = dynamic_cast<reports::test::AddressableValue*>(report);
  ASSERT_NE(nullptr, avreport);

  auto report_values = avreport->values();
  ASSERT_EQ(15u, report_values.size());
  // Validate that the values are within expected bounds (mean ± 3*sigma for normal distribution)
  for (unsigned year = 1994; year <= 1996; ++year) {
    double value = report_values[year];
    EXPECT_GE(value, 2.0);  // mean - 3*sigma
    EXPECT_LE(value, 8.0);  // mean + 3*sigma
  }

  EXPECT_NEAR(4.4459810702527038, report_values[1994], 1e-9);
  EXPECT_NEAR(3.7494286151769622, report_values[1995], 1e-9);
  EXPECT_NEAR(4.0524417553765524, report_values[1996], 1e-9);
  EXPECT_NEAR(5.4475535298818425, report_values[1997], 1e-9);
  EXPECT_NEAR(4.2931056963305938, report_values[1998], 1e-9);
  EXPECT_NEAR(5.1442491304429803, report_values[1999], 1e-9);
  EXPECT_NEAR(3.7169158191124718, report_values[2000], 1e-9);
  EXPECT_NEAR(8.0, report_values[2001], 1e-9);
}

TEST_F(InternalEmptyModel, Timevarying_RandomDraw_Lognormal) {
  const string extra_config = R"(
    @time_varying random_draw
    type random_draw
    parameter selectivity[FishingSel].a50
    years  1994 1995 1996 1997 1998 1999 2000
    mean 5.0
    sigma 1.0
    lower_bound 1
    upper_bound 8
    distribution lognormal
  
    @report test_report
    type addressable_value
    time_step step_two
    addressable selectivity[FishingSel].a50
    )";

  AddConfigurationLine(niwa::testresources::models::two_sex, "TestResources/Models/TwoSex.h", 31);
  AddConfigurationLine(extra_config, __FILE__, 30);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  auto* time_varying = model_->managers()->time_varying()->get("random_draw");
  ASSERT_NE(nullptr, time_varying);

  RandomDraw* random_draw = dynamic_cast<RandomDraw*>(time_varying);
  ASSERT_NE(nullptr, random_draw);

  Report* report = model_->managers()->report()->get("test_report");
  ASSERT_NE(nullptr, report);

  reports::test::AddressableValue* avreport = dynamic_cast<reports::test::AddressableValue*>(report);
  ASSERT_NE(nullptr, avreport);

  auto report_values = avreport->values();
  ASSERT_EQ(15u, report_values.size());
  // Validate that the values are within expected bounds (mean ± 3*sigma for normal distribution)
  for (unsigned year = 1994; year <= 1996; ++year) {
    double value = report_values[year];
    EXPECT_GE(value, 2.0);  // mean - 3*sigma
    EXPECT_LE(value, 8.0);  // mean + 3*sigma
  }

  EXPECT_NEAR(4.3934220247383999, report_values[1994], 1e-9);
  EXPECT_NEAR(3.8273078584129063, report_values[1995], 1e-9);
  EXPECT_NEAR(4.0640135159736168, report_values[1996], 1e-9);
  EXPECT_NEAR(5.3573103122965353, report_values[1997], 1e-9);
  EXPECT_NEAR(4.2624011532336814, report_values[1998], 1e-9);
  EXPECT_NEAR(5.0449864655731771, report_values[1999], 1e-9);
  EXPECT_NEAR(3.8027433532588288, report_values[2000], 1e-9);
  EXPECT_NEAR(8.0, report_values[2001], 1e-9);
}

}  // namespace niwa::timevarying
#endif  // TESTMODE