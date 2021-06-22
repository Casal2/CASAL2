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

#include "../../DerivedQuantities/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../MPD/MPD.Mock.h"
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

using niwa::MockMPD;
using std::cout;
using std::endl;
using ::testing::NiceMock;

class HamiltonianMonteCarloThreadedModel : public testfixtures::BaseThreaded {};

/**
 * Notes for these unit tests.
 *
 * Because we're using an arctan transformation in the deltadiff minimiser we end up with quite
 * different values between Operating Systems. Each of these unit tests will #ifdef the Operating
 * System so we can check the values between them.
 *
 * Arguably, all answers are correct. The difference starts off <1e-16 but when we use this to inform
 * the gradient it does change the direction of the leap frog slightly. After some jumps we end up
 * in a place that will fail a unit test, even though it's arguably correct.
 *
 * Second note:
 *  The code I use to print the results
 */
//  {
//   cout << std::setprecision(16);
//   for (unsigned i = 0; i < chain.size(); i += 10) cout << "EXPECT_DOUBLE_EQ(chain[" << i << "].score_, " << chain[i].score_ << ");" << endl;
// }

/**
 * @brief Test the HMC MCMC algorithm with the TwoSex model doing Five Iterations
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1978.519054583444);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1978.519228249186);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1978.519264474865);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1978.519379675362);
#elif __linux__
  EXPECT_DOUBLE_EQ(chain[0].score_, 1979.31910941548);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1979.318837290028);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1979.318332128152);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1979.318110301643);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1979.317733207503);
#endif
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

#ifdef _WIN64
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
#elif __linux__
  EXPECT_DOUBLE_EQ(chain[0].score_, 1979.31910941548);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1979.31910667998);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1979.319101457041);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1979.319099189367);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1979.319095221592);
  EXPECT_DOUBLE_EQ(chain[5].score_, 1979.319093918872);
  EXPECT_DOUBLE_EQ(chain[6].score_, 1979.319090717183);
  EXPECT_DOUBLE_EQ(chain[7].score_, 1979.319102191839);
  EXPECT_DOUBLE_EQ(chain[8].score_, 1979.319099816649);
  EXPECT_DOUBLE_EQ(chain[9].score_, 1979.319095325929);
  EXPECT_DOUBLE_EQ(chain[10].score_, 1979.31909364716);
  EXPECT_DOUBLE_EQ(chain[11].score_, 1979.319090029647);
  EXPECT_DOUBLE_EQ(chain[12].score_, 1979.319090616388);
  EXPECT_DOUBLE_EQ(chain[13].score_, 1979.319087741061);
  EXPECT_DOUBLE_EQ(chain[14].score_, 1979.319081412415);
  EXPECT_DOUBLE_EQ(chain[15].score_, 1979.319078851855);
  EXPECT_DOUBLE_EQ(chain[16].score_, 1979.319074174167);
  EXPECT_DOUBLE_EQ(chain[17].score_, 1979.319072148373);
  EXPECT_DOUBLE_EQ(chain[18].score_, 1979.319068518474);
  EXPECT_DOUBLE_EQ(chain[19].score_, 1979.319068194253);
  EXPECT_DOUBLE_EQ(chain[20].score_, 1979.319065299487);
  EXPECT_DOUBLE_EQ(chain[21].score_, 1979.319054702343);
  EXPECT_DOUBLE_EQ(chain[22].score_, 1979.319051302415);
  EXPECT_DOUBLE_EQ(chain[23].score_, 1979.319052394097);
  EXPECT_DOUBLE_EQ(chain[24].score_, 1979.319049773885);
#endif
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

#ifdef _WIN64
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
#elif __linux__
  EXPECT_DOUBLE_EQ(chain[0].score_, 1979.31910941548);
  EXPECT_DOUBLE_EQ(chain[5].score_, 1979.319093918872);
  EXPECT_DOUBLE_EQ(chain[10].score_, 1979.31909364716);
  EXPECT_DOUBLE_EQ(chain[15].score_, 1979.319078851855);
  EXPECT_DOUBLE_EQ(chain[20].score_, 1979.319065299487);
  EXPECT_DOUBLE_EQ(chain[25].score_, 1979.319044239775);
  EXPECT_DOUBLE_EQ(chain[30].score_, 1979.319036663157);
  EXPECT_DOUBLE_EQ(chain[35].score_, 1979.319034464879);
  EXPECT_DOUBLE_EQ(chain[40].score_, 1979.318998175371);
  EXPECT_DOUBLE_EQ(chain[45].score_, 1979.318943928067);
  EXPECT_DOUBLE_EQ(chain[50].score_, 1979.318915751307);
  EXPECT_DOUBLE_EQ(chain[55].score_, 1979.318904315214);
  EXPECT_DOUBLE_EQ(chain[60].score_, 1979.318878531083);
  EXPECT_DOUBLE_EQ(chain[65].score_, 1979.318864756771);
  EXPECT_DOUBLE_EQ(chain[70].score_, 1979.318853744845);
  EXPECT_DOUBLE_EQ(chain[75].score_, 1979.318836001205);
  EXPECT_DOUBLE_EQ(chain[80].score_, 1979.31882112822);
  EXPECT_DOUBLE_EQ(chain[85].score_, 1979.318808247617);
  EXPECT_DOUBLE_EQ(chain[90].score_, 1979.318785268916);
  EXPECT_DOUBLE_EQ(chain[95].score_, 1979.318774377177);
#endif
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5352753193411);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.599099544303);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.6008751998088);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.6008751998088);
#elif __linux__
  // EXPECT_NEAR(chain[0].score_, 487.520638972034, 1e-7);
  // EXPECT_NEAR(chain[1].score_, 487.5352752521567, 1e-7);
  // EXPECT_NEAR(chain[2].score_, 487.6219696273498, 1e-7);
  // EXPECT_NEAR(chain[3].score_, 487.6233820105855, 1e-7);
  // EXPECT_NEAR(chain[4].score_, 487.6233820105855, 1e-7);
#endif
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5396500365894);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.6088597505599);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.6156285077633);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.6156285077633);
#elif __linux__
  // EXPECT_NEAR(chain[0].score_, 487.520638972034, 1e-5);
  // EXPECT_NEAR(chain[1].score_, 487.5396497473042, 1e-5);
  // EXPECT_NEAR(chain[2].score_, 487.6318753298668, 1e-5);
  // EXPECT_NEAR(chain[3].score_, 487.6383538202512, 1e-5);
  // EXPECT_NEAR(chain[4].score_, 487.6383538202512, 1e-5);
#endif
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 487.5206389722261);
  EXPECT_DOUBLE_EQ(chain[1].score_, 487.5208286195056);
  EXPECT_DOUBLE_EQ(chain[2].score_, 487.520890677574);
  EXPECT_DOUBLE_EQ(chain[3].score_, 487.5209464835007);
  EXPECT_DOUBLE_EQ(chain[4].score_, 487.5210030419244);
#elif __linux__
  EXPECT_NEAR(chain[0].score_, 487.520638972034, 1e-5);
  EXPECT_NEAR(chain[1].score_, 487.5208286193689, 1e-5);
  EXPECT_NEAR(chain[2].score_, 487.5208929731917, 1e-5);
  EXPECT_NEAR(chain[3].score_, 487.5209487497292, 1e-5);
  EXPECT_NEAR(chain[4].score_, 487.5210056161228, 1e-5);
#endif
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[1].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[2].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[3].score_, 12179.97032733484);
  EXPECT_DOUBLE_EQ(chain[4].score_, 12179.97032733484);
#elif __linux__
  // EXPECT_NEAR(chain[0].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[1].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[2].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[3].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[4].score_, 11622.04009177895, 1e-5);
#endif
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

#ifdef _WIN64
  EXPECT_DOUBLE_EQ(chain[0].score_, 1978.518998268854);
  EXPECT_DOUBLE_EQ(chain[1].score_, 1978.519054583445);
  EXPECT_DOUBLE_EQ(chain[2].score_, 1978.519228249186);
  EXPECT_DOUBLE_EQ(chain[3].score_, 1978.519264474865);
  EXPECT_DOUBLE_EQ(chain[4].score_, 1978.519379675368);
#elif __linux__
  EXPECT_NEAR(chain[0].score_, 1979.31910941548, 1e-5);
  EXPECT_NEAR(chain[1].score_, 1979.318837290026, 1e-5);
  EXPECT_NEAR(chain[2].score_, 1979.318332128151, 1e-5);
  EXPECT_NEAR(chain[3].score_, 1979.318110301642, 1e-5);
  EXPECT_NEAR(chain[4].score_, 1979.317733207502, 1e-5);
#endif
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

#ifdef _WIN64
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
#elif __linux__
  EXPECT_NEAR(chain[0].score_, 36389.07677669402, 1e-5);
  EXPECT_NEAR(chain[1].score_, 36389.07677669402, 1e-5);
  EXPECT_NEAR(chain[2].score_, 36388.361465831, 1e-5);
  EXPECT_NEAR(chain[3].score_, 36388.361465831, 1e-5);
  EXPECT_NEAR(chain[4].score_, 36388.361465831, 1e-5);
  EXPECT_NEAR(chain[5].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[6].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[7].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[8].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[9].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[10].score_, 36388.8173073483, 1e-5);
  EXPECT_NEAR(chain[11].score_, 36388.50090547473, 1e-5);
  EXPECT_NEAR(chain[12].score_, 36374.96143373416, 1e-5);
  EXPECT_NEAR(chain[13].score_, 36365.27657489545, 1e-5);
  EXPECT_NEAR(chain[14].score_, 36322.78711909857, 1e-5);
  EXPECT_NEAR(chain[15].score_, 36322.48103413592, 1e-5);
  EXPECT_NEAR(chain[16].score_, 36322.90573281318, 1e-5);
  EXPECT_NEAR(chain[17].score_, 36324.69622726875, 1e-5);
  EXPECT_NEAR(chain[18].score_, 36322.25413765675, 1e-5);
  EXPECT_NEAR(chain[19].score_, 36323.19781703433, 1e-5);
  EXPECT_NEAR(chain[20].score_, 36312.8207311583, 1e-5);
  EXPECT_NEAR(chain[21].score_, 36315.46101229153, 1e-5);
  EXPECT_NEAR(chain[22].score_, 36311.54510717453, 1e-5);
  EXPECT_NEAR(chain[23].score_, 36326.95904361141, 1e-5);
  EXPECT_NEAR(chain[24].score_, 36320.59445996349, 1e-5);
#endif
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

#ifdef _WIN64
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
#elif __linux__
  // EXPECT_NEAR(chain[0].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[1].score_, 11620.01985703409, 1e-5);
  // EXPECT_NEAR(chain[2].score_, 11618.73341844001, 1e-5);
  // EXPECT_NEAR(chain[3].score_, 11618.21271520679, 1e-5);
  // EXPECT_NEAR(chain[4].score_, 11618.21271520679, 1e-5);
  // EXPECT_NEAR(chain[5].score_, 11600.91618645375, 1e-5);
  // EXPECT_NEAR(chain[6].score_, 11591.3493038226, 1e-5);
  // EXPECT_NEAR(chain[7].score_, 11600.90538041638, 1e-5);
  // EXPECT_NEAR(chain[8].score_, 11600.90538041638, 1e-5);
  // EXPECT_NEAR(chain[9].score_, 11601.31245975959, 1e-5);
  // EXPECT_NEAR(chain[10].score_, 11589.56936167276, 1e-5);
  // EXPECT_NEAR(chain[11].score_, 11594.77573869354, 1e-5);
  // EXPECT_NEAR(chain[12].score_, 11582.39226485891, 1e-5);
  // EXPECT_NEAR(chain[13].score_, 11582.72638786315, 1e-5);
  // EXPECT_NEAR(chain[14].score_, 11576.40774277365, 1e-5);
  // EXPECT_NEAR(chain[15].score_, 11581.47904821901, 1e-5);
  // EXPECT_NEAR(chain[16].score_, 11591.86020479118, 1e-5);
  // EXPECT_NEAR(chain[17].score_, 11586.80098401977, 1e-5);
  // EXPECT_NEAR(chain[18].score_, 11588.94000961911, 1e-5);
  // EXPECT_NEAR(chain[19].score_, 11588.94000961911, 1e-5);
  // EXPECT_NEAR(chain[20].score_, 11584.2109864414, 1e-5);
  // EXPECT_NEAR(chain[21].score_, 11577.90015058309, 1e-5);
  // EXPECT_NEAR(chain[22].score_, 11577.90015058309, 1e-5);
  // EXPECT_NEAR(chain[23].score_, 11563.32968127872, 1e-5);
  // EXPECT_NEAR(chain[24].score_, 11556.61019993286, 1e-5);
#endif
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

#ifdef _WIN64
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
#elif __linux__
  // EXPECT_NEAR(chain[0].score_, 11622.04009177895, 1e-5);
  // EXPECT_NEAR(chain[1].score_, 11616.98912320913, 1e-5);
  // EXPECT_NEAR(chain[2].score_, 11612.66679924271, 1e-5);
  // EXPECT_NEAR(chain[3].score_, 11609.11006547069, 1e-5);
  // EXPECT_NEAR(chain[4].score_, 11609.11006547069, 1e-5);
  // EXPECT_NEAR(chain[5].score_, 11588.81821244274, 1e-5);
  // EXPECT_NEAR(chain[6].score_, 11576.18072649907, 1e-5);
  // EXPECT_NEAR(chain[7].score_, 11582.66232534397, 1e-5);
  // EXPECT_NEAR(chain[8].score_, 11602.24104623353, 1e-5);
  // EXPECT_NEAR(chain[9].score_, 11599.60566906656, 1e-5);
  // EXPECT_NEAR(chain[10].score_, 11584.84484506599, 1e-5);
  // EXPECT_NEAR(chain[11].score_, 11587.00651477895, 1e-5);
  // EXPECT_NEAR(chain[12].score_, 11571.63800588351, 1e-5);
  // EXPECT_NEAR(chain[13].score_, 11568.92469216468, 1e-5);
  // EXPECT_NEAR(chain[14].score_, 11559.61656781061, 1e-5);
  // EXPECT_NEAR(chain[15].score_, 11561.62636514807, 1e-5);
  // EXPECT_NEAR(chain[16].score_, 11568.94428881099, 1e-5);
  // EXPECT_NEAR(chain[17].score_, 11560.86209234402, 1e-5);
  // EXPECT_NEAR(chain[18].score_, 11559.94823220482, 1e-5);
  // EXPECT_NEAR(chain[19].score_, 11622.22043635682, 1e-5);
  // EXPECT_NEAR(chain[20].score_, 11614.3868750975, 1e-5);
  // EXPECT_NEAR(chain[21].score_, 11604.94834979492, 1e-5);
  // EXPECT_NEAR(chain[22].score_, 11604.94834979492, 1e-5);
  // EXPECT_NEAR(chain[23].score_, 11587.16841312723, 1e-5);
  // EXPECT_NEAR(chain[24].score_, 11577.35793797639, 1e-5);
#endif
}

}  // namespace niwa
#endif  // USE_AUTODIFF
#endif  // TESTMODE
