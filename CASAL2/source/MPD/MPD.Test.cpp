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

} /* namespace niwa */

#endif /* TESTMODE */
