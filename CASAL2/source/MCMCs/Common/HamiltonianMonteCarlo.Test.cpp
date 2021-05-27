/**
 * @file HamiltonianMonteCarlo.Test.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief
 * @version 0.1
 * @date 2021-05-10
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifdef TESTMODE
#ifndef USE_AUTODIFF

// Headers
#include <iomanip>
#include <iostream>

#include "../../ConfigurationLoader/MPD.Mock.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/TestCases/CasalComplex1.h"
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

class HamiltonianMonteCarloThreadedModel : public testfixtures::BaseThreaded {};

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_TwoSex) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 0
    keep 1
    leapfrog_delta 1e-5
    leapfrog_steps 3
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1978.519054583444);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1978.519228249186);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1978.519264474865);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1978.519379675362);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, TwentyFive_Iteration_With_TwoSex) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "threads 1", "threads 2");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 25
    start 0
    keep 1
    leapfrog_delta 1e-7
    leapfrog_steps 3
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(25u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1978.518998817745);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1978.51900038332);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1978.519000696139);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1978.519001650357);
  EXPECT_DOUBLE_EQ(chain[5].score_, 1978.519001523487);
  EXPECT_DOUBLE_EQ(chain[6].score_, 1978.519002095521);
  EXPECT_DOUBLE_EQ(chain[7].score_, 1978.51899535043);
  EXPECT_DOUBLE_EQ(chain[8].score_, 1978.518995445497);
  EXPECT_DOUBLE_EQ(chain[9].score_, 1978.518995961152);
  EXPECT_DOUBLE_EQ(chain[10].score_, 1978.518995646324);
  EXPECT_DOUBLE_EQ(chain[11].score_, 1978.518996556224);
  EXPECT_DOUBLE_EQ(chain[12].score_, 1978.518994690515);
  EXPECT_DOUBLE_EQ(chain[13].score_, 1978.518995193622);
  EXPECT_DOUBLE_EQ(chain[14].score_, 1978.518996633549);
  EXPECT_DOUBLE_EQ(chain[15].score_, 1978.51899689487);
  EXPECT_DOUBLE_EQ(chain[16].score_, 1978.518997807235);
  EXPECT_DOUBLE_EQ(chain[17].score_, 1978.518997615353);
  EXPECT_DOUBLE_EQ(chain[18].score_, 1978.518998115631);
  EXPECT_DOUBLE_EQ(chain[19].score_, 1978.518988730297);
  EXPECT_DOUBLE_EQ(chain[20].score_, 1978.518989137762);
  EXPECT_DOUBLE_EQ(chain[21].score_, 1978.518996897601);
  EXPECT_DOUBLE_EQ(chain[22].score_, 1978.518997167277);
  EXPECT_DOUBLE_EQ(chain[23].score_, 1978.518999663646);
  EXPECT_DOUBLE_EQ(chain[24].score_, 1978.518999787179);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, OneHundred_Iteration_With_TwoSex) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 100
    start 0
    keep 1
    leapfrog_delta 1e-7
    leapfrog_steps 3
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(100u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[5].score_, 1978.519001523487);
  EXPECT_DOUBLE_EQ(chain[10].score_, 1978.518995646324);
  EXPECT_DOUBLE_EQ(chain[15].score_, 1978.51899689487);
  EXPECT_DOUBLE_EQ(chain[20].score_, 1978.518989137762);
  EXPECT_DOUBLE_EQ(chain[25].score_, 1978.519001515965);
  EXPECT_DOUBLE_EQ(chain[30].score_, 1978.519005714694);
  EXPECT_DOUBLE_EQ(chain[35].score_, 1978.519058520261);
  EXPECT_DOUBLE_EQ(chain[40].score_, 1978.51905906475);
  EXPECT_DOUBLE_EQ(chain[45].score_, 1978.519063378923);
  EXPECT_DOUBLE_EQ(chain[50].score_, 1978.519064995936);
  EXPECT_DOUBLE_EQ(chain[55].score_, 1978.519083705551);
  EXPECT_DOUBLE_EQ(chain[60].score_, 1978.519079419952);
  EXPECT_DOUBLE_EQ(chain[65].score_, 1978.519076355686);
  EXPECT_DOUBLE_EQ(chain[70].score_, 1978.519086422208);
  EXPECT_DOUBLE_EQ(chain[75].score_, 1978.519112608864);
  EXPECT_DOUBLE_EQ(chain[80].score_, 1978.519111450098);
  EXPECT_DOUBLE_EQ(chain[85].score_, 1978.519112390293);
  EXPECT_DOUBLE_EQ(chain[90].score_, 1978.519110547037);
  EXPECT_DOUBLE_EQ(chain[95].score_, 1978.519109207944);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_CasalComplexOne) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 8");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 0
    keep 1
    leapfrog_delta 1e-5
    leapfrog_steps 3
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5352753193411);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.599099544303);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.6008751998088);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.6008751998088);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_CasalComplexOne_LeapFrog_Steps_Ten) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 0
    keep 1
    leapfrog_delta 1e-5
    leapfrog_steps 10
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5396500365894);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.6088597505599);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.6156285077633);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.6156285077633);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_CasalComplexOne_LeapFrog_Delta_OneESeven) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 0
    keep 1
    leapfrog_delta 1e-7
    leapfrog_steps 10
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5208286195056);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.520890677574);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.5209464835007);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.5210030419244);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_CasalComplexOne_LeapFrog_Delta_OneEThree_RandomStart) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 1
    keep 1
    leapfrog_delta 1e-3
    leapfrog_steps 10
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[1].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[2].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[3].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[4].score_, 12179.97032733484);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, Five_Iteration_With_TwoSex_CustomGradientStepSize) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, __LINE__);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 5
    start 0
    keep 1
    leapfrog_delta 1e-5
    leapfrog_steps 3
    gradient_step_size 1e-9
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, __LINE__);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(5u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1978.519054583445);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1978.519228249186);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1978.519264474865);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1978.519379675368);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, TwentyFive_Iteration_With_TwoSex_RandomStart_OneEFourStepSize) {
  string ammended_definition = testcases::test_cases_two_sex_model_population;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, 76);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 25
    start 1
    keep 1
    leapfrog_delta 1e-4
    leapfrog_steps 5
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, 70);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(25u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 29585.64866161223);
  EXPECT_DOUBLE_EQ(chain[1].score_, 29583.01830473165);
  EXPECT_DOUBLE_EQ(chain[2].score_, 29580.3292307786);
  EXPECT_DOUBLE_EQ(chain[3].score_, 29578.87724471112);
  EXPECT_DOUBLE_EQ(chain[4].score_, 29580.13208585848);
  EXPECT_DOUBLE_EQ(chain[5].score_, 29577.98699965725);
  EXPECT_DOUBLE_EQ(chain[6].score_, 29584.42041933127);
  EXPECT_DOUBLE_EQ(chain[7].score_, 29586.60872298994);
  EXPECT_DOUBLE_EQ(chain[8].score_, 29586.60872298994);
  EXPECT_DOUBLE_EQ(chain[9].score_, 29586.60872298994);
  EXPECT_DOUBLE_EQ(chain[10].score_, 29574.53187205235);
  EXPECT_DOUBLE_EQ(chain[11].score_, 29475.36458465407);
  EXPECT_DOUBLE_EQ(chain[12].score_, 29481.61009781449);
  EXPECT_DOUBLE_EQ(chain[13].score_, 29477.0018145502);
  EXPECT_DOUBLE_EQ(chain[14].score_, 29499.12675181811);
  EXPECT_DOUBLE_EQ(chain[15].score_, 29499.02744166505);
  EXPECT_DOUBLE_EQ(chain[16].score_, 29479.86126152759);
  EXPECT_DOUBLE_EQ(chain[17].score_, 29483.89903154561);
  EXPECT_DOUBLE_EQ(chain[18].score_, 29473.26054788286);
  EXPECT_DOUBLE_EQ(chain[19].score_, 29473.50597506662);
  EXPECT_DOUBLE_EQ(chain[20].score_, 29473.56723235059);
  EXPECT_DOUBLE_EQ(chain[21].score_, 29480.57181824369);
  EXPECT_DOUBLE_EQ(chain[22].score_, 29473.7079144983);
  EXPECT_DOUBLE_EQ(chain[23].score_, 29291.7513114489);
  EXPECT_DOUBLE_EQ(chain[24].score_, 29384.39532536846);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, TwentyFive_Iteration_With_CasalComplexOne_RandomStart_OneEFourStepSize) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, 76);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 25
    start 1
    keep 1
    leapfrog_delta 1e-4
    leapfrog_steps 4
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, 70);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(25u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[1].score_, 12177.92540373069);
  EXPECT_DOUBLE_EQ(chain[2].score_, 12176.66255928856);
  EXPECT_DOUBLE_EQ(chain[3].score_, 12176.93611931805);
  EXPECT_DOUBLE_EQ(chain[4].score_, 12176.93611931805);
  EXPECT_DOUBLE_EQ(chain[5].score_, 12157.74380680738);
  EXPECT_DOUBLE_EQ(chain[6].score_, 12157.74380680738);
  EXPECT_DOUBLE_EQ(chain[7].score_, 12166.15198374331);
  EXPECT_DOUBLE_EQ(chain[8].score_, 12166.15198374331);
  EXPECT_DOUBLE_EQ(chain[9].score_, 12153.96381474516);
  EXPECT_DOUBLE_EQ(chain[10].score_, 12138.42768991613);
  EXPECT_DOUBLE_EQ(chain[11].score_, 12146.29438455936);
  EXPECT_DOUBLE_EQ(chain[12].score_, 12138.57199309062);
  EXPECT_DOUBLE_EQ(chain[13].score_, 12140.24032774361);
  EXPECT_DOUBLE_EQ(chain[14].score_, 12135.17751974745);
  EXPECT_DOUBLE_EQ(chain[15].score_, 12141.72746403739);
  EXPECT_DOUBLE_EQ(chain[16].score_, 12149.29025525479);
  EXPECT_DOUBLE_EQ(chain[17].score_, 12142.22891680645);
  EXPECT_DOUBLE_EQ(chain[18].score_, 12151.7462507854);
  EXPECT_DOUBLE_EQ(chain[19].score_, 12151.7462507854);
  EXPECT_DOUBLE_EQ(chain[20].score_, 12152.85246080102);
  EXPECT_DOUBLE_EQ(chain[21].score_, 12146.89581480627);
  EXPECT_DOUBLE_EQ(chain[22].score_, 12130.92975201858);
  EXPECT_DOUBLE_EQ(chain[23].score_, 12104.14897393031);
  EXPECT_DOUBLE_EQ(chain[24].score_, 12096.81609686514);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, TwentyFive_Iteration_With_CasalComplexOne_RandomStart_OneEFourStepSize_LeapFrogs_Ten) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 4");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, 76);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 25
    start 1
    keep 1
    leapfrog_delta 1e-4
    leapfrog_steps 10
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, 70);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(25u, chain.size());

  EXPECT_DOUBLE_EQ(chain[0].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[1].score_, 12174.87274154569);
  EXPECT_DOUBLE_EQ(chain[2].score_, 12170.55080165687);
  EXPECT_DOUBLE_EQ(chain[3].score_, 12167.76250525961);
  EXPECT_DOUBLE_EQ(chain[4].score_, 12167.76250525961);
  EXPECT_DOUBLE_EQ(chain[5].score_, 12145.55172252662);
  EXPECT_DOUBLE_EQ(chain[6].score_, 12172.67109075281);
  EXPECT_DOUBLE_EQ(chain[7].score_, 12178.07361909592);
  EXPECT_DOUBLE_EQ(chain[8].score_, 12178.07361909592);
  EXPECT_DOUBLE_EQ(chain[9].score_, 12162.837605144);
  EXPECT_DOUBLE_EQ(chain[10].score_, 12144.19279304693);
  EXPECT_DOUBLE_EQ(chain[11].score_, 12149.0263007111);
  EXPECT_DOUBLE_EQ(chain[12].score_, 12138.20593354141);
  EXPECT_DOUBLE_EQ(chain[13].score_, 12136.8288863702);
  EXPECT_DOUBLE_EQ(chain[14].score_, 12128.72535134095);
  EXPECT_DOUBLE_EQ(chain[15].score_, 12132.21819963118);
  EXPECT_DOUBLE_EQ(chain[16].score_, 12136.69125936036);
  EXPECT_DOUBLE_EQ(chain[17].score_, 12126.59738988044);
  EXPECT_DOUBLE_EQ(chain[18].score_, 12133.03767581772);
  EXPECT_DOUBLE_EQ(chain[19].score_, 12133.03767581772);
  EXPECT_DOUBLE_EQ(chain[20].score_, 12131.02483954309);
  EXPECT_DOUBLE_EQ(chain[21].score_, 12122.04507343575);
  EXPECT_DOUBLE_EQ(chain[22].score_, 12103.1699895665);
  EXPECT_DOUBLE_EQ(chain[23].score_, 12073.54432225372);
  EXPECT_DOUBLE_EQ(chain[24].score_, 12063.21039403586);
}

/**
 * @brief Construct a new test f object
 *
 */
TEST_F(HamiltonianMonteCarloThreadedModel, OneHundred_Iteration_With_CasalComplexOne_RandomStart_OneEFourStepSize_LeapFrogs_Ten) {
  string ammended_definition = testcases::test_cases_casal_complex_1;
  boost::replace_all(ammended_definition, "threads 1", "threads 16");
  boost::replace_all(ammended_definition, "numerical_differences", "deltadiff");
  AddConfigurationLine(ammended_definition, __FILE__, 76);

  string mcmc_definition = R"(
    @mcmc my_mcmc
    type hamiltonian
    length 100
    start 1
    keep 1
    leapfrog_delta 1e-4
    leapfrog_steps 10
  )";
  AddConfigurationLine(mcmc_definition, __FILE__, 70);
  LoadConfiguration();

  ASSERT_NO_THROW(runner_->GoWithRunMode(RunMode::kMCMC));
  auto model = runner_->model();
  auto mcmc  = model->managers()->mcmc()->active_mcmc();
  ASSERT_TRUE(mcmc != nullptr);

  auto chain = mcmc->chain();
  ASSERT_EQ(100u, chain.size());

  // {
  //   cout << std::setprecision(16);
  //   for (unsigned i = 0; i < chain.size(); i += 10) cout << "EXPECT_DOUBLE_EQ(chain[" << i << "].score_, " << chain[i].score_ << ");" << endl;
  // }

  EXPECT_DOUBLE_EQ(chain[0].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[10].score_, 12144.19279304693);
  EXPECT_DOUBLE_EQ(chain[20].score_, 12131.02483954309);
  EXPECT_DOUBLE_EQ(chain[30].score_, 12052.2572662629);
  EXPECT_DOUBLE_EQ(chain[40].score_, 12022.88065427561);
  EXPECT_DOUBLE_EQ(chain[50].score_, 11933.03808419933);
  EXPECT_DOUBLE_EQ(chain[60].score_, 11893.56589301748);
  EXPECT_DOUBLE_EQ(chain[70].score_, 11822.49082378147);
  EXPECT_DOUBLE_EQ(chain[80].score_, 11748.11135254011);
  EXPECT_DOUBLE_EQ(chain[90].score_, 11645.16527931475);
}

}  // namespace niwa
#endif  // USE_AUTODIFF
#endif  // TESTMODE
