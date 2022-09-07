/**
 * @file Constant.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "../../DerivedQuantities/Manager.h"
#include "../../Model/Models/Age.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Projects/Manager.h"
#include "../../TestResources/Models/CasalComplex1.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "Constant.h"

// Namespaces
namespace niwa {
namespace projects {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/*
 * Input projection
 */
const string constant_ycs =
    R"(
@project future_ycs
type constant
parameter process[recruitment].recruitment_multipliers
years 2001:2012
values 0.478482 0.640663 0.640091 0.762361 0.560125 0.651637 0.764833 0.645498 0.678341 1.234 1.0423 1.4352
)";

/**
 * Run test
 */
TEST_F(InternalEmptyModel, Projects_Constant_YCS) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  AddConfigurationLine(constant_ycs, __FILE__, 35);
  LoadConfiguration();
  model_->Start(RunMode::kProjection);

  Project* project = model_->managers()->project()->GetProject("future_ycs");
  if (!project)
    LOG_FATAL() << "!project";

  DerivedQuantity* dq = model_->managers()->derived_quantity()->GetDerivedQuantity("ssb");
  if (!dq)
    LOG_FATAL() << "!dq";

  // test the values have changed
  map<unsigned, Double>& values   = project->projected_parameters();
  vector<double>         expected = {0.478482, 0.640663, 0.640091, 0.762361, 0.560125, 0.651637, 0.764833, 0.645498, 0.678341, 1.234, 1.0423, 1.4352};
  unsigned               iter     = 0;
  for (auto value : values) {
    EXPECT_DOUBLE_EQ(expected[iter], value.second);
    ++iter;
  }
  // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14270661.55, 14557416, 14192873.25, 13415898.85, 12671105.73, 12078964.29, 11569986.83, 11146711.2, 10868327.73, 10755924.97};
  for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);  // << " for year " << year << " and value " << Expect[i];
  }
}

/**
 *	Throw an exception as not projected ycs block
 */
TEST_F(InternalEmptyModel, Projects_Throw_Exception) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kProjection), std::string);
}

const string future_catch =
    R"(
@project future_catch
type constant
parameter process[fishing].catches
years 2003:2012
values 4000
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_Constant_Catches) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  AddConfigurationLine(constant_ycs, __FILE__, 35);
  AddConfigurationLine(future_catch, __FILE__, 35);

  LoadConfiguration();
  model_->Start(RunMode::kProjection);

  Project* project = model_->managers()->project()->GetProject("future_catch");
  if (!project)
    LOG_FATAL() << "!project";

  DerivedQuantity* dq = model_->managers()->derived_quantity()->GetDerivedQuantity("ssb");
  if (!dq)
    LOG_FATAL() << "!dq";

  // test the values have changed
  map<unsigned, Double>& values = project->projected_parameters();
  unsigned               iter   = 0;
  for (auto value : values) {
    EXPECT_DOUBLE_EQ(4000, value.second);
    ++iter;
  }
  // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14270661.55, 14553924.97, 14186439.76, 13407035.26, 12660251.63, 12066479.12, 11556163.21, 11131787.38, 10852493.85, 10739344.23};
  for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);  // << " for year " << year << " and value " << Expect[i];
  }
}
} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
