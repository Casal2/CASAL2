/**
 * @file MPD.Test.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Test our MPD loader
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE

// Headers
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <iostream>

#include "../Estimates/Estimate.h"
#include "../Estimates/Manager.h"
#include "../MCMCs/MCMC.h"
#include "../MCMCs/Manager.h"
#include "../Minimisers/Manager.h"
#include "../Minimisers/Minimiser.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../Processes/Manager.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TestResources/TestCases/TwoSexModel.h"
#include "../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../Utilities/String.h"
#include "MPD.Mock.h"
#include "MPD.h"

// Namespaces
namespace niwa {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 * This test will load an MPD file form a string (above) using
 * the configurationloader::mpd class. We'll then load the
 * estimate values and covariance back and make sure they're
 * as we expect.
 */
TEST_F(InternalEmptyModel, MPDLoader_TwoSex_MPD) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 1
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, 71);
  LoadConfiguration();

  model_->global_configuration().set_estimate_before_mcmc(true);
  model_->Start(RunMode::kTesting);

  ASSERT_EQ(1u, model_->managers()->mcmc()->size());

  model_->set_run_mode(RunMode::kMCMC);
  MockMPD mpd(model_);
  mpd.ParseString(TwoSex_MPD);
  mpd.ParseFile();

  // Validate the Estimate values now
  auto estimate = model_->managers()->estimate()->GetEstimate("catchability[CPUEq].q");
  ASSERT_TRUE(estimate);
  EXPECT_DOUBLE_EQ(estimate->value(), 1.10693e-05);

  estimate = model_->managers()->estimate()->GetEstimate("process[Recruitment].R0");
  ASSERT_TRUE(estimate);
  EXPECT_DOUBLE_EQ(estimate->value(), 2.37219e+06);

  estimate = model_->managers()->estimate()->GetEstimate("selectivity[FishingSel].a50");
  ASSERT_TRUE(estimate);
  EXPECT_DOUBLE_EQ(estimate->value(), 10.1887);

  estimate = model_->managers()->estimate()->GetEstimate("selectivity[FishingSel].ato95");
  ASSERT_TRUE(estimate);
  EXPECT_DOUBLE_EQ(estimate->value(), 4.89097);

  auto mcmc = model_->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc);
  auto covariance = mcmc->covariance_matrix();
  ASSERT_EQ(4u, covariance.size1());
  ASSERT_EQ(4u, covariance.size2());

  EXPECT_DOUBLE_EQ(covariance(0, 0), 2.13934e-12);
  EXPECT_DOUBLE_EQ(covariance(0, 1), -0.321068);
  EXPECT_DOUBLE_EQ(covariance(0, 2), -4.45289e-09);
  EXPECT_DOUBLE_EQ(covariance(0, 3), -9.74773e-10);

  EXPECT_DOUBLE_EQ(covariance(1, 0), -0.321068);
  EXPECT_DOUBLE_EQ(covariance(1, 1), 6.699e+10);
  EXPECT_DOUBLE_EQ(covariance(1, 2), -519.28);
  EXPECT_DOUBLE_EQ(covariance(1, 3), -143.374);

  EXPECT_DOUBLE_EQ(covariance(2, 0), -4.45289e-09);
  EXPECT_DOUBLE_EQ(covariance(2, 1), -519.28);
  EXPECT_DOUBLE_EQ(covariance(2, 2), 0.0123193);
  EXPECT_DOUBLE_EQ(covariance(2, 3), 0.0100754);

  EXPECT_DOUBLE_EQ(covariance(3, 0), -9.74773e-10);
  EXPECT_DOUBLE_EQ(covariance(3, 1), -143.374);
  EXPECT_DOUBLE_EQ(covariance(3, 2), 0.0100754);
  EXPECT_DOUBLE_EQ(covariance(3, 3), 0.0129463);
}

/**
 * The MPD text is the only way the estimate values and covariance cross from the
 * minimiser's library to the MCMC library. CreateMPD must write them at full
 * precision so ParseString recovers exactly the same doubles.
 */
TEST_F(InternalEmptyModel, MPD_CreateMPD_RoundTripsFullPrecision) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  // The TwoSex test case already defines the active (numerical_differences) minimiser
  string definitions = R"(
    @mcmc my_mcmc
    type random_walk
    length 1
  )";
  AddConfigurationLine(definitions, __FILE__, 160);
  LoadConfiguration();

  model_->global_configuration().set_estimate_before_mcmc(true);
  model_->Start(RunMode::kTesting);

  vector<string> labels = {"catchability[CPUEq].q", "process[Recruitment].R0", "selectivity[FishingSel].a50", "selectivity[FishingSel].ato95"};
  vector<double> values = {1.2345678901234567e-05, 2371234.5678901234, 10.188712345678901, 4.8909712345678901};
  auto           estimates = model_->managers()->estimate();
  ASSERT_EQ(labels.size(), estimates->GetIsEstimatedCount());
  for (unsigned i = 0; i < labels.size(); ++i) {
    auto estimate = estimates->GetEstimate(labels[i]);
    ASSERT_TRUE(estimate);
    estimate->set_value(values[i]);
  }

  auto minimiser = model_->managers()->minimiser()->active_minimiser();
  ASSERT_TRUE(minimiser);
  auto& minimiser_covariance = minimiser->covariance_matrix();
  minimiser_covariance.resize(labels.size(), labels.size());
  for (unsigned i = 0; i < labels.size(); ++i)
    for (unsigned j = 0; j < labels.size(); ++j) minimiser_covariance(i, j) = 1.0 / (3.0 + i + j) + (i == j ? 1e-7 / 3.0 : 0.0);

  MockMPD writer(model_);
  writer.CreateMPD(model_);

  // Move the estimates away so the parse has to restore them
  for (const string& label : labels) estimates->GetEstimate(label)->set_value(1.0);

  model_->set_run_mode(RunMode::kMCMC);
  MockMPD reader(model_);
  reader.ParseString(writer.value());

  // Exact equality: any rounding in the text would show up here
  for (unsigned i = 0; i < labels.size(); ++i) EXPECT_EQ(values[i], estimates->GetEstimate(labels[i])->value()) << labels[i];

  auto& covariance = model_->managers()->mcmc()->active_mcmc()->covariance_matrix();
  ASSERT_EQ(labels.size(), covariance.size1());
  ASSERT_EQ(labels.size(), covariance.size2());
  for (unsigned i = 0; i < labels.size(); ++i)
    for (unsigned j = 0; j < labels.size(); ++j) EXPECT_EQ(minimiser_covariance(i, j), covariance(i, j)) << "(" << i << "," << j << ")";
}

} /* namespace niwa */

#endif /* TESTMODE */
