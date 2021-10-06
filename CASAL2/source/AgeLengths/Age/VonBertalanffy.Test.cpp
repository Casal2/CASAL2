/**
 * @file VonBertalanffy.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "VonBertalanffy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../../Partition/Partition.h"
#include "../../Selectivities/Common/Logistic.h"
#include "../../TestResources/MockClasses/Managers.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/Distribution.h"
// namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockVonBertalanffy : public VonBertalanffy {
public:
  MockVonBertalanffy(Distribution distribution = Distribution::kNormal) : VonBertalanffy(nullptr) { distribution_ = distribution; }

  MockVonBertalanffy(shared_ptr<Model> model, Double linf, Double k, Double t0, bool by_length, Double cv_first, Double cv_last, vector<Double> time_step_proportions,
                     string compatibility = PARAM_CASAL2, Distribution distribution = Distribution::kNormal) :
      VonBertalanffy(model) {
    linf_                  = linf;
    k_                     = k;
    t0_                    = t0;
    by_length_             = by_length;
    cv_first_              = cv_first;
    cv_last_               = cv_last;
    time_step_proportions_ = time_step_proportions;
    compatibility_         = compatibility;
    distribution_          = distribution; 
    year_offset_ = model ->start_year();
    age_offset_ = model ->min_age();    
    min_age_ = model ->min_age(); 
    max_age_ = model ->min_age(); 
    model_years_ = model->years();
    age_length_matrix_years_ = model->years();
    for(unsigned i = 0; i < age_length_matrix_years_.size(); ++i) {
      age_length_matrix_year_key_[age_length_matrix_years_[i]] = i;
    }
    // allocate memory for cvs; this is usually done in the Build() but difficult to mock
    cvs_.resize(model->years().size());
    for(unsigned year_ndx = 0; year_ndx < cvs_.size(); ++year_ndx) {
      cvs_[year_ndx].resize(model->time_steps().size());
      for(unsigned time_step_ndx = 0; time_step_ndx < cvs_[year_ndx].size(); ++time_step_ndx) {
        cvs_[year_ndx][time_step_ndx].resize(model->age_spread(), 0.0);
      }
    }   

    age_length_transition_matrix_.resize(age_length_matrix_years_.size());
    for(unsigned year_iter = 0; year_iter < age_length_matrix_years_.size(); ++year_iter) {
      age_length_transition_matrix_[year_iter].resize(model->time_steps().size());
      for (unsigned time_step_ndx = 0; time_step_ndx < model->time_steps().size(); ++time_step_ndx) {
        age_length_transition_matrix_[year_iter][time_step_ndx].resize(model_->age_spread());
        numbers_by_age_length_transition_.resize(model_->age_spread());
        for(unsigned age_iter = 0; age_iter < model_->age_spread(); ++age_iter) {
          age_length_transition_matrix_[year_iter][time_step_ndx][age_iter].resize(model_->length_bins().size(), 0.0);
          numbers_by_age_length_transition_[age_iter].resize(model_->get_number_of_length_bins(), 0.0);
        }
      }
    }             
  }

  void MockPopulateCV() { this->PopulateCV(); }
  void MockPopulateAgeLengthMatrix() { this->PopulateAgeLengthMatrix(); }
  void MockUpdateAgeLengthMatrixForThisYear(unsigned year) { this->UpdateAgeLengthMatrixForThisYear(year); }
  // to validate elements of the age-length transition
  const Double Mock_get_age_length_probability(unsigned year, unsigned time_step, unsigned age, unsigned length_bin_ndx) { return age_length_transition_matrix_[age_length_matrix_year_key_[year]][time_step][age - age_offset_][length_bin_ndx];}
  void change_k(Double new_k) { k_ = new_k;}

};

TEST(AgeLengths, VonBertalanffy_MeanLength) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0}, PARAM_CASAL2, Distribution::kNormal);
  von_bertalanffy.MockPopulateCV();

  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990,0, 1));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990,0, 2));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990,0, 3));
  EXPECT_DOUBLE_EQ(4.9596000375416427, von_bertalanffy.calculate_mean_length(1990, 0, 4));
  EXPECT_DOUBLE_EQ(9.611729668428497, von_bertalanffy.calculate_mean_length(1990, 0, 5));
  EXPECT_DOUBLE_EQ(13.975450520665413, von_bertalanffy.calculate_mean_length(1990, 0, 6));
  EXPECT_DOUBLE_EQ(18.068642496620129, von_bertalanffy.calculate_mean_length(1990, 0, 7));
  EXPECT_DOUBLE_EQ(21.908077034104725, von_bertalanffy.calculate_mean_length(1990, 0, 8));
  EXPECT_DOUBLE_EQ(25.509485825636233, von_bertalanffy.calculate_mean_length(1990, 0, 9));
  EXPECT_DOUBLE_EQ(28.887625277446702, von_bertalanffy.calculate_mean_length(1990, 0, 10));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

TEST(AgeLengths, VonBertalanffy_MeanLength_2) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0}, PARAM_CASAL2, Distribution::kLogNormal);
  von_bertalanffy.MockPopulateCV();

  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990, 0, 1));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990, 0, 2));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.calculate_mean_length(1990, 0, 3));
  EXPECT_DOUBLE_EQ(4.9596000375416427, von_bertalanffy.calculate_mean_length(1990,0, 4));
  EXPECT_DOUBLE_EQ(9.611729668428497, von_bertalanffy.calculate_mean_length(1990,0, 5));
  EXPECT_DOUBLE_EQ(13.975450520665413, von_bertalanffy.calculate_mean_length(1990,0, 6));
  EXPECT_DOUBLE_EQ(18.068642496620129, von_bertalanffy.calculate_mean_length(1990,0, 7));
  EXPECT_DOUBLE_EQ(21.908077034104725, von_bertalanffy.calculate_mean_length(1990,0, 8));
  EXPECT_DOUBLE_EQ(25.509485825636233, von_bertalanffy.calculate_mean_length(1990,0, 9));
  EXPECT_DOUBLE_EQ(28.887625277446702, von_bertalanffy.calculate_mean_length(1990,0, 10));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/**
 *
 */
TEST(AgeLengths, VonBertalanffy_CV) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0},  PARAM_CASAL2, Distribution::kNormal);
  von_bertalanffy.MockPopulateCV();

  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 1));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 2));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 3));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 4));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 8));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/**
 *
 */
TEST(AgeLengths, VonBertalanffy_CV_2) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->bind_calls();
  MockVonBertalanffy von_bertalanffy(model, 169.07, 0.093, -0.256, true, 0.102, 0.0, {0.0, 0.0}, PARAM_CASAL, Distribution::kNormal);
  von_bertalanffy.MockPopulateCV();
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 1));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 2));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 3));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 4));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 8));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/*
* test the building of the age-length transition matrix
*/
TEST(AgeLengths, BuildAgeLengthProportions) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_plus(true);
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0, 1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateCV());
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateAgeLengthMatrix());

  unsigned min_age = model->min_age();
  for (unsigned year = 0; year < 1; ++year) {
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 0 + min_age, 0));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 0 + min_age, 1));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0,0 + min_age, 2));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0,0 + min_age, 3));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 0 + min_age, 4));

    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 2 + min_age, 0));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 2 + min_age, 1));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 2 + min_age, 2));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 2 + min_age, 3));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 2 + min_age, 4));

    EXPECT_DOUBLE_EQ(0.41996720731752746, von_bertalanffy.Mock_get_age_length_probability(year,0, 4 + min_age, 0));
    EXPECT_DOUBLE_EQ(3.2667977767353307e-008, von_bertalanffy.Mock_get_age_length_probability(year,0, 4 + min_age, 1));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 4 + min_age, 2));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 4 + min_age, 3));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 4 + min_age, 4));

    EXPECT_DOUBLE_EQ(0.69070308538891911, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.29603445809402762, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 1));
    EXPECT_DOUBLE_EQ(0.00048060422206530617, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 2));
    EXPECT_DOUBLE_EQ(6.4636329621947652e-010, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 3));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 4));

    EXPECT_DOUBLE_EQ(0.13891252421751288, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.67051952644369806, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 1));
    EXPECT_DOUBLE_EQ(0.18713059299787771, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 2));
    EXPECT_DOUBLE_EQ(0.0022533864694446182, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 3));
    EXPECT_DOUBLE_EQ(7.9325922641704238e-007, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 4));
  }
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}


/*
* check RebuildCache functionality() from the time-varying class
*/

TEST(AgeLengths, VonBertalanffy_change_k) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_plus(true);
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0, 1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateCV());
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateAgeLengthMatrix());

  unsigned min_age = model->min_age();
  for (unsigned year = 0; year < 1; ++year) {
    EXPECT_DOUBLE_EQ(0.69070308538891911, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.29603445809402762, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 1));
    EXPECT_DOUBLE_EQ(0.00048060422206530617, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 2));
    EXPECT_DOUBLE_EQ(6.4636329621947652e-010, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 3));
    EXPECT_DOUBLE_EQ(0u, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 4));

    EXPECT_DOUBLE_EQ(0.13891252421751288, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.67051952644369806, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 1));
    EXPECT_DOUBLE_EQ(0.18713059299787771, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 2));
    EXPECT_DOUBLE_EQ(0.0022533864694446182, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 3));
    EXPECT_DOUBLE_EQ(7.9325922641704238e-007, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 4));
  }
  // change K
  von_bertalanffy.change_k(0.1);
  von_bertalanffy.MockPopulateCV();
  von_bertalanffy.MockUpdateAgeLengthMatrixForThisYear(1990);
  min_age = model->min_age();
  for (unsigned year = 0; year < 1; ++year) {
    EXPECT_DOUBLE_EQ(0.11248435140284607, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.64062560348154041, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 1));
    EXPECT_DOUBLE_EQ( 0.24104036867781808, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 2));
    EXPECT_DOUBLE_EQ(0.0048919529778737036, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 3));
    EXPECT_DOUBLE_EQ(3.7549792077928856e-06, von_bertalanffy.Mock_get_age_length_probability(year,0, 6 + min_age, 4));

    EXPECT_DOUBLE_EQ(0.012738662003135892, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 0));
    EXPECT_DOUBLE_EQ(0.18635967735434533, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 1));
    EXPECT_DOUBLE_EQ(0.50647102919617581, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 2));
    EXPECT_DOUBLE_EQ(0.26723777001670812, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 3));
    EXPECT_DOUBLE_EQ(0.027042517702221103, von_bertalanffy.Mock_get_age_length_probability(year,0, 8 + min_age, 4));
  }

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}


/**
 * This method will test the BuildAgeLengthProportions  method
 */
TEST(AgeLengths, BuildAgeLengthProportions_normal) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->set_length_plus(true);
  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, -1.16415, false, 0.1, 0.1, {1.0, 1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateCV());
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateAgeLengthMatrix());

  EXPECT_DOUBLE_EQ(35.498582852610163, von_bertalanffy.calculate_mean_length(1990,0, 7));
  // double mu = 35.49858; // for the growth model in the mock class this is equivalant to mean age of an 7 year old
  // due to not being able to have the exact same expected value as previous test all the expected values changed
  // I didn't know how they were generated so created new expected values based on R see below to reproduce
  /*
  * New expected values are generated by R using the following code
  * R version 4.0.2 (2020-06-22)
  * Platform: x86_64-w64-mingw32/x64 (64-bit)
  * Running under: Windows 10 x64 (build 18363)
   mu = 35.498582852610163
   cv = 0.1
   sd = mu * cv
   len_bins = c(0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47)
   expect_probs = vector();
   expect_probs[1] = pnorm(len_bins[2], mu, sd)
   for(i in 1:(length(len_bins) - 1)) {
    expect_probs[i] = pnorm(len_bins[i + 1], mu, sd) - pnorm(len_bins[i], mu, sd) 
   }
   expect_probs[length(len_bins)] = 1 - pnorm(len_bins[length(len_bins)], mu, sd) 
   round(expect_probs, 7)
  */
  vector<Double> expected = {0, 0, 1e-07, 3e-07, 1.3e-06, 4.6e-06, 1.58e-05, 4.95e-05, 0.0001435, 0.0003843, 0.0009515, 0.0021771, 0.0046035, 0.0089963, 0.0162479, 0.0271202, 0.0418358, 0.0596439, 0.0785862, 0.0956949, 0.1076949, 0.1120121, 0.1076709, 0.0956522, 0.0785335, 0.0595906, 0.0417891, 0.0270839, 0.0162225, 0.0089802, 0.0045942, 0.0021722, 0.0009492, 0.0005977};
  
  // not check 8 year olds based on the previous mean age test
  Double total = 0.0;
  for (unsigned i = 0; i < expected.size(); ++i) {
    total += von_bertalanffy.Mock_get_age_length_probability(1990,0, 7, i);
    EXPECT_NEAR(expected[i], von_bertalanffy.Mock_get_age_length_probability(1990,0, 7, i), 0.00001) << " with i = " << i;
  }
  EXPECT_DOUBLE_EQ(1.0, total);

}

/**
 * This method will test the BuildAgeLengthProportions  method
 */
TEST(AgeLengths, BuildAgeLengthProportions_lognormal) {
  
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->set_length_plus(true);
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, -1.16415, false, 0.1, 0.1, {1.0, 1.0}, PARAM_CASAL2, Distribution::kLogNormal);
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateCV());
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateAgeLengthMatrix());

  EXPECT_DOUBLE_EQ(35.498582852610163, von_bertalanffy.calculate_mean_length(1990,0, 7));
  // double mu = 35.49858; // for the growth model in the mock class this is equivalant to mean age of an 7 year old
  // due to not being able to have the exact same expected value as previous test all the expected values changed
  // I didn't know how they were generated so created new expected values based on R see below to reproduce
  /*
  * New expected values are generated by R using the following code
  * R version 4.0.2 (2020-06-22)
  * Platform: x86_64-w64-mingw32/x64 (64-bit)
  * Running under: Windows 10 x64 (build 18363)
  mu = 35.498582852610163
  cv = 0.1
  sd = sqrt(log(cv * cv + 1.0))
  mu_adj = log(mu) - sd^2 / 2.0
  len_bins = c(0.0001, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47)
  log_len_bins = log(len_bins)
  expect_probs = vector();
  expect_probs[1] = pnorm(log_len_bins[2], mu_adj, sd)
  for(i in 1:(length(len_bins) - 1)) {
    expect_probs[i] = pnorm(log_len_bins[i + 1], mu_adj, sd) - pnorm(log_len_bins[i], mu_adj, sd) 
  }
  expect_probs[length(len_bins)] = 1 - pnorm(log_len_bins[length(len_bins)], mu_adj, sd) 
  round(expect_probs, 7)
  */
  vector<Double> expected =  {0, 0, 0, 0, 0, 0, 1e-07, 9e-07, 7.5e-06, 4.5e-05, 0.0002116, 0.0007986, 0.0024716, 0.0063963, 0.0140816, 0.0267734, 0.0445554, 0.0656767, 0.0866653, 0.1033531, 0.1123421, 0.112157, 0.1035552, 0.0889789, 0.0715525, 0.0541267, 0.0386966, 0.0262574, 0.016976, 0.0104946, 0.0062238, 0.0035514, 0.0019551, 0.0020955};
  
  // not check 8 year olds based on the previous mean age test
  Double total = 0.0;
  for (unsigned i = 0; i < expected.size(); ++i) {
    total += von_bertalanffy.Mock_get_age_length_probability(1990,0, 7, i);
    EXPECT_NEAR(expected[i], von_bertalanffy.Mock_get_age_length_probability(1990,0, 7, i), 0.00001) << " with i = " << i;
  }
  EXPECT_DOUBLE_EQ(1.0, total);
  
}

}  // namespace agelengths
} /* namespace niwa */
#endif
