/**
 * @file Selectivity.Test.cpp
 * @brief Tests for the base Selectivity length-based (age-length integrated) results
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifdef TESTMODE

#include "Selectivity.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "../AgeLengths/Age/VonBertalanffy.h"
#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TimeSteps/Manager.h"
#include "Common/Logistic.h"

namespace niwa {
using ::testing::Return;
using ::testing::ReturnRef;

namespace {
class LengthBasedTestTimeStepManager : public timesteps::Manager {
public:
  unsigned current_time_step() const override final { return 0; }
};

/**
 * An age-length with fixed mean lengths and cvs, so the selectivity integration
 * can be checked without running a model
 */
class FixedLognormalAgeLength : public agelengths::VonBertalanffy {
public:
  FixedLognormalAgeLength(shared_ptr<Model> model, const vector<Double>& mean_lengths, const vector<Double>& cvs, unsigned year) : VonBertalanffy(model) {
    distribution_label_ = PARAM_LOGNORMAL;
    year_offset_        = year;
    age_offset_         = model->min_age();
    time_step_offset_   = 0;
    mean_length_by_timestep_age_.assign(1, mean_lengths);
    cvs_.assign(1, vector<vector<Double>>(1, cvs));
  }
};

}  // namespace

/**
 * The lognormal length-at-age branch of GetLengthBasedResult is written as
 * exp(mu + sigma * z_j) so it keeps derivatives in autodiff builds. It must
 * give the same values as the boost lognormal quantile it replaced.
 */
TEST(Selectivities, LengthBased_LognormalMatchesBoostQuantiles) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  MockManagersPtr       mock_managers = MockManagersPtr(new MockManagers(model));
  LengthBasedTestTimeStepManager time_step_manager;
  vector<string>                 time_steps = {"step1"};
  vector<unsigned>               years      = {1995};

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(6));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1995));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1995));
  EXPECT_CALL(*model, current_year()).WillRepeatedly(Return(1995));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  vector<Double> mean_lengths = {31.0, 38.5, 44.2, 48.9, 52.6, 55.4};
  vector<Double> cvs          = {0.25, 0.2, 0.18, 0.15, 0.12, 0.1};
  const double   a50 = 45.0, ato95 = 30.0;
  const unsigned n_quant = 7;

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  FixedLognormalAgeLength       age_length(model, mean_lengths, cvs, 1995);
  niwa::selectivities::Logistic selectivity(model);
  selectivity.parameters().Add(PARAM_LABEL, "length_logistic", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_TYPE, "logistic", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_A50, "45", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_ATO95, "30", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_LENGTH_BASED, "true", __FILE__, __LINE__);
  selectivity.parameters().Add(PARAM_INTERVALS, "7", __FILE__, __LINE__);
  selectivity.Validate();

  for (unsigned age = 5; age <= 10; ++age) {
    double mean  = AS_DOUBLE(mean_lengths[age - 5]);
    double cv    = AS_DOUBLE(cvs[age - 5]);
    double sigma = std::sqrt(std::log(1.0 + cv * cv));
    double mu    = std::log(mean) - sigma * sigma * 0.5;
    boost::math::lognormal dist{mu, sigma};

    double expected = 0.0;
    for (unsigned j = 1; j <= n_quant; ++j) {
      double size = boost::math::quantile(dist, (j - 0.5) / n_quant);
      expected += 1.0 / (1.0 + std::pow(19.0, (a50 - size) / ato95));
    }
    expected /= n_quant;

    EXPECT_NEAR(expected, AS_DOUBLE(selectivity.GetAgeResult(age, &age_length)), 1e-12) << "age=" << age;
  }
}

}  // namespace niwa
#endif  // TESTMODE
