/**
 * @file MCMC.Test.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifdef TESTMODE

// Headers
#include "../MCMCs/MCMC.h"

#include <iostream>

#include "../DerivedQuantities/Manager.h"
#include "../MCMCs/Manager.h"
#include "../MPD/MPD.Mock.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TestResources/TestCases/TwoSexModel.h"
#include "../TestResources/TestFixtures/BaseThreaded.h"
#include "../TestResources/TestFixtures/BasicModel.h"
#include "MCMC.Mock.h"
#include "MCMC.h"
#include "Manager.h"

// Namespaces
namespace niwa {

using niwa::MockMPD;
using std::cout;
using std::endl;
using ::testing::NiceMock;

// Empty class to give us better printing of Unit test results
class MCMCThreadedModel : public testfixtures::BaseThreaded {};

/**
 *
 */
TEST_F(MCMCThreadedModel, MCMC_1Iteration_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 1
    keep 1
  )";

  AddConfigurationLine(mcmc_definition, __FILE__, 71);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(1u, chain.size());
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(MCMCThreadedModel, MCMC_2Iterations_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 2
    keep 1
  )";

  AddConfigurationLine(mcmc_definition, __FILE__, 71);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(2u, chain.size());
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(MCMCThreadedModel, MCMC_25Iterations_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 25
    keep 1
  )";

  AddConfigurationLine(mcmc_definition, __FILE__, 71);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(25u, chain.size());
}

}  // namespace niwa
#endif  // TESTMODE