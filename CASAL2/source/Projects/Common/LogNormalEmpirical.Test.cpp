/**
 * @file LogNormalEmpirical.Test.cpp
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
#include "LogNormalEmpirical.h"

// Namespaces
namespace niwa {
namespace projects {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const string LogNormalEmpirical_ycs =
    R"(
@project future_ycs
type lognormal_empirical
parameter process[recruitment].recruitment_multipliers
years 2001:2012
start_year 1977
final_year 2001
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_LogNormal_Empirical_YCS) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  AddConfigurationLine(LogNormalEmpirical_ycs, __FILE__, 35);
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
  vector<double>         expected = {1.193159531211873,   0.76433685650904026, 0.59278375352171586, 0.66209045366492325, 1.1015707349908905,  0.72286526934850714,
                             0.98615503786499570, 0.58579237843666654, 0.80899806415388953, 0.83630043479857519, 0.76944498598285171, 0.72614647686888445};
  vector<unsigned> proj_years = {2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012};
  for (unsigned yr_ndx = 0; yr_ndx < proj_years.size(); ++yr_ndx) {
    EXPECT_DOUBLE_EQ(expected[yr_ndx], values[proj_years[yr_ndx]]);
  }
  // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14416960.80, 15029708.91, 15241070.46, 14841433.57, 14129449.26, 13489008.60, 13203140.65, 12994232.07, 12755996.14, 12438217.98};
  for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);  // << " for year " << year << " and value " << Expect[i];
  }
}

} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
