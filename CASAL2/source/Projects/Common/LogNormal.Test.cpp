/**
 * @file LogNormal.Test.cpp
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
#include "../../Utilities/RandomNumberGenerator.h"
#include "LogNormal.h"

// Namespaces
namespace niwa {
namespace projects {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const string LogNormal_ycs =
    R"(
@project future_ycs
type lognormal
parameter process[recruitment].recruitment_multipliers
years 2001:2012
sigma 1.2
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_LogNormal_YCS) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 28);
  AddConfigurationLine(LogNormal_ycs, __FILE__, 35);
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
  vector<double>         expected = {0.59470516037518018, 0.1231198954127662,  4.4223030240241812,  0.87871321335550301, 0.18646856678311435, 0.11909713899382668,
                             1.0829707265041799,  0.25036818220616419, 0.10853466523372596, 0.15612943666532977, 0.83282165103240524, 0.20840475255359467};
  unsigned               iter     = 0;
  for (auto value : values) {
    EXPECT_DOUBLE_EQ(expected[iter], value.second);
    ++iter;
  }
  // test the final SSB's haven't changed, this will ensure as well as saving the parameters the parameters in the underlying system have changed as well
  vector<double> Expect = {14294453.21, 14524240.72, 14802669.39, 15306948.26, 17112346.68, 18073583.53, 16981920.2, 15163751.16, 13717890.51, 12354198.6};
  for (unsigned i = 0; i < 10; ++i) {
    unsigned year = 2002 + i;
    EXPECT_NEAR(Expect[i], dq->GetValue(year), 1e-2);  // << " for year " << year << " and value " << Expect[i];
  }
}

} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
