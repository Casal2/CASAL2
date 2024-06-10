/**
 * @file EmpiricalSampling.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include "../../Utilities/RandomNumberGenerator.h"
#include "EmpiricalSampling.h"

// Namespaces
namespace niwa {
namespace projects {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const string EmpiricalSampling_ycs =
    R"(
@project future_ycs
type empirical_sampling
parameter process[recruitment].recruitment_multipliers
years 2001:2012
start_year 1977
final_year 2001
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_Empirical_Sampling_YCS) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  AddConfigurationLine(EmpiricalSampling_ycs, __FILE__, 35);
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
  vector<double>         expected = {1.014, 0.9450, 0.9340, 0.9450, 0.9340, 1.325, 1.0487, 0.3520, 1.345, 1.5434, 0.946, 1.0487};
  vector<unsigned> proj_years = {2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012};
  for (unsigned yr_ndx = 0; yr_ndx < proj_years.size(); ++yr_ndx) {
    EXPECT_DOUBLE_EQ(expected[yr_ndx], values[proj_years[yr_ndx]]);
  }
  // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14380285.66, 14954609.60, 15177987.29, 15033001.02, 14756909.92, 14567939.81, 14524441.87, 14549946.66, 14490135.35, 14239342.24};
  for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);
  }
}

}  // namespace projects
} /* namespace niwa */

#endif /* ifdef TESTMODE */
