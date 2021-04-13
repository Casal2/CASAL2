/**
 * @file Abundance.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 5/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "TimeStep.h"

#include <iostream>

#include "Model/Factory.h"
#include "Observations/Manager.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace age {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;


// from TestResources/TestFixtures/BasicModel.cpp
const std::string test_cases_observation_abundance =
R"(
@model
min_age 1
max_age 20
age_plus t
start_year 1994
final_year 2008
base_weight_units kgs
time_steps step_one

@categories
format stage.sex
names immature.male mature.male immature.female mature.female


@time_step step_one
processes ageing recruitment mortality

@ageing ageing
categories immature.male immature.female

@recruitment recruitment
type constant
categories immature.male immature.female
proportions 0.6 0.4
r0 100000
age 1

@mortality mortality
type constant_rate
categories immature.male immature.female mature.male mature.female
m 0.065
relative_m_by_age constant_one

@selectivity constant_one
type constant
c 1

@catchability catchability
type free
q 0.000153139

@observation abundance
type abundance
catchability catchability
time_step step_one
categories immature.male+immature.female immature.female
selectivities constant_one constant_one constant_one
likelihood lognormal
time_step_proportion 1.0
years 2008
table obs
2008 22.50 11.25 0.2
end_table

@report DQ
type derived_quantity

)";

/**
 *
 */
/*
TEST_F(BasicModel, Observation_Abundance) {

  // Recruitment process
  vector<string> recruitment_categories   = { "immature.male", "immature.female" };
  vector<string> proportions  = { "0.6", "0.4" };
  base::Object* process = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT);
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  // Mortality process
  vector<string> mortality_categories   = { "immature.male", "immature.female", "mature.male", "mature.female" };
  process = model_->factory().CreateObject(PARAM_MORTALITY, PARAM_CONSTANT_RATE);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant_rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_M, "0.065", __FILE__, __LINE__);
  process->parameters().Add(PARAM_RELATIVE_M_BY_AGE, "constant_one", __FILE__, __LINE__);

  // Report process
  base::Object* report = model_->factory().CreateObject(PARAM_REPORT, PARAM_DERIVED_QUANTITY);
  ASSERT_NE(report, nullptr);
  report->parameters().Add(PARAM_LABEL, "DQ", __FILE__, __LINE__);
  report->parameters().Add(PARAM_TYPE, "derived_quantity", __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories   = { "immature.male", "immature.female" };
  process = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  vector<string> processes    = { "ageing", "recruitment", "mortality" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  // Catchability
  base::Object* catchability = model_->factory().CreateObject(PARAM_CATCHABILITY, PARAM_FREE);
  catchability->parameters().Add(PARAM_LABEL, "catchability", __FILE__, __LINE__);
  catchability->parameters().Add(PARAM_TYPE, "free", __FILE__, __LINE__);
  catchability->parameters().Add(PARAM_Q, "0.000153139", __FILE__, __LINE__);

  // Observation
  vector<string> observation_categories = { "immature.male+immature.female", "immature.female" };
  vector<string> obs = { "2008", "22.50", "11.25", "0.2" };
  vector<string> selectivities = { "constant_one", "constant_one", "constant_one" };
  base::Object* observation = model_->factory().CreateObject(PARAM_OBSERVATION, PARAM_ABUNDANCE);
  observation->parameters().Add(PARAM_LABEL, "abundance", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_TYPE, "abundance", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_CATCHABILITY, "catchability", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_YEARS, "2008", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_TIME_STEP, "step_one", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_CATEGORIES, observation_categories, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_SELECTIVITIES, selectivities, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_OBS, obs, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_LIKELIHOOD, "lognormal", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_TIME_STEP_PROPORTION, "1.0", __FILE__, __LINE__);
  */

TEST_F(InternalEmptyModel, Observation_Abundance) {
  AddConfigurationLine(test_cases_observation_abundance, __FILE__, 32);
  LoadConfiguration();

  model_->Start(RunMode::kTesting);
  model_->FullIteration();

  const vector<obs::Comparison>& comparisons = model_->managers()->observation()->GetObservation("abundance")->comparisons(2008);
  ASSERT_EQ(2u, comparisons.size());

  EXPECT_EQ("immature.male+immature.female", comparisons[0].category_);
  EXPECT_DOUBLE_EQ(0.2, comparisons[0].error_value_);
  EXPECT_DOUBLE_EQ(142.01537476494462, comparisons[0].expected_);
  EXPECT_DOUBLE_EQ(22.5, comparisons[0].observed_);
  EXPECT_DOUBLE_EQ(40.738892086047329, comparisons[0].score_);

  EXPECT_EQ("immature.female", comparisons[1].category_);
  EXPECT_DOUBLE_EQ(0.2, comparisons[1].error_value_);
  EXPECT_DOUBLE_EQ(56.806149905977861, comparisons[1].expected_);
  EXPECT_DOUBLE_EQ(11.25, comparisons[1].observed_);
  EXPECT_DOUBLE_EQ(31.002921785658106, comparisons[1].score_);
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
