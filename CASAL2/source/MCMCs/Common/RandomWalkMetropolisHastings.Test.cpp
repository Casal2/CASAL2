/**
 * @file RandomWalkMetropolisHastings.Test.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-04-25
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE
#ifndef USE_AUTODIFF

// Headers
#include <iostream>

#include "../../ConfigurationLoader/MPD.Mock.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/TestCases/TwoSexModel.h"
#include "../../TestResources/TestFixtures/BaseThreaded.h"
#include "../../TestResources/TestFixtures/BasicModel.h"
#include "../MCMC.Mock.h"
#include "../MCMC.h"
#include "../Manager.h"

// Namespaces
namespace niwa {

using niwa::configuration::MockMPD;
using std::cout;
using std::endl;
using ::testing::NiceMock;

class RandomWalkMetropolisHastingsThreadedModel : public testfixtures::BaseThreaded {};

/**
 * @brief Test loading a simple parameter into the MCMC
 * class and then validating the object to ensure everything
 * works as expected
 *
 */
TEST(RandomWalkMetropolisHastings, Validate_Defaults) {
  // TODO: This isn't actually testing RWMH
  shared_ptr<MockModel> model = std::make_shared<MockModel>();
  shared_ptr<MCMC>      mcmc  = std::make_shared<MockMCMC>(model);
  ASSERT_TRUE(model);
  ASSERT_TRUE(mcmc);

  ParameterList& parameters = mcmc->parameters();
  parameters.Add(PARAM_LABEL, "TestMCMC", __FILE__, __LINE__);
  parameters.Add(PARAM_TYPE, PARAM_RANDOMWALK, __FILE__, __LINE__);
  parameters.Add(PARAM_LENGTH, "1234", __FILE__, __LINE__);

  ASSERT_NO_THROW(mcmc->Validate());
}

/**
 *
 */
TEST_F(RandomWalkMetropolisHastingsThreadedModel, One_Iteration_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 1
    start 1
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

  ASSERT_DOUBLE_EQ(chain[0].score_, 4233.9275238745377);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(RandomWalkMetropolisHastingsThreadedModel, One_Iteration_With_TwoSex_NoRNGStart) {
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

  ASSERT_DOUBLE_EQ(chain[0].score_, 1993.8041770617783);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(RandomWalkMetropolisHastingsThreadedModel, Two_Iterations_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type random_walk
    length 2
    start 1
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
TEST_F(RandomWalkMetropolisHastingsThreadedModel, TwentyFive_Iterations_With_TwoSex) {
  AddConfigurationLine(testcases::test_cases_two_sex_model_population, __FILE__, 27);

  string mcmc_definition = R"(
    @mcmc my_mcmc
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
#endif  // USE_AUTODIFF
#endif  // TESTMODE
