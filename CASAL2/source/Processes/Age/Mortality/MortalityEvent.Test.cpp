/**
 * @file MortalityEvent.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 5/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "Model/Factory.h"
#include "Model/Managers.h"
#include "MortalityEventRemoval.h"
#include "Partition/Partition.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/BasicModel.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::testfixtures::BasicModel;
using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

/**
 *
 */
TEST_F(BasicModel, Processes_Mortality_Event_No_Penalty) {
  // Recruitment process
  vector<string> recruitment_categories = {"immature.male", "immature.female"};
  vector<string> proportions            = {"0.6", "0.4"};
  base::Object*  process                = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT);
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  // Report process
  base::Object* report = model_->factory().CreateObject(PARAM_REPORT, PARAM_DERIVED_QUANTITY);
  ASSERT_NE(report, nullptr);
  report->parameters().Add(PARAM_LABEL, "DQ", __FILE__, __LINE__);
  report->parameters().Add(PARAM_TYPE, "derived_quantity", __FILE__, __LINE__);

  // Mortality process
  vector<string> mortality_categories = {"immature.male", "immature.female", "mature.male", "mature.female"};
  vector<string> years                = {"1998", "1999", "2000", "2001", "2002", "2003", "2004"};
  vector<string> catches              = {"1000", "2000", "3000", "5000", "8000", "13000", "21000"};
  vector<string> selectivities        = {"constant_one", "constant_one", "constant_one", "constant_one"};
  process                             = model_->factory().CreateObject(PARAM_MORTALITY, PARAM_EVENT);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "event", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_YEARS, years, __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATCHES, catches, __FILE__, __LINE__);
  process->parameters().Add(PARAM_U_MAX, "0.99", __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, selectivities, __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories = {"immature.male", "immature.female"};
  process                          = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  niwa::base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  vector<string>      processes = {"ageing", "recruitment", "mortality"};
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  model_->Start(RunMode::kTesting);

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");
  partition::Category& mature_male     = model_->partition().category("mature.male");
  partition::Category& mature_female   = model_->partition().category("mature.female");

  // Verify blank partition
  for (unsigned i = 0; i < immature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 0; i < immature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  for (unsigned i = 0; i < mature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;

  /**
   * Do 1 iteration of the model then check the categories to see if
   * the maturation was successful
   */
  model_->FullIteration();

  vector<double> immature_males   = {60000,
                                   60000,
                                   60000,
                                   60000,
                                   58820.224719101127,
                                   58040.751812527917,
                                   57518.450333899993,
                                   57156.243467817498,
                                   56910.233811571517,
                                   56720.216336407662,
                                   56606.775903734859,
                                   56606.775903734859,
                                   56606.775903734859,
                                   56606.775903734859,
                                   56606.775903734859};
  vector<double> immature_females = {40000,
                                     40000,
                                     40000,
                                     40000,
                                     39213.483146067418,
                                     38693.834541685283,
                                     38345.633555933331,
                                     38104.162311878339,
                                     37940.155874381009,
                                     37813.477557605118,
                                     37737.850602489903,
                                     37737.850602489903,
                                     37737.850602489903,
                                     37737.850602489903,
                                     37737.850602489903};

  for (unsigned i = 0; i < immature_male.data_.size() && i < 15; ++i) {
    EXPECT_DOUBLE_EQ(immature_males[i], immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
    EXPECT_DOUBLE_EQ(immature_females[i], immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  }

  // Mature should still be 0 because we had no maturation
  for (unsigned i = 0; i < mature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;
}


/**
 * Catch far exceeds the vulnerable stock, so u_max binds in every year. Exercises the u_max cap,
 * the penalty trigger and the actual_catches/exploitation_rate report values -- none of which the
 * test above reaches. Expected values were captured from the pre-TBM-3 code.
 */
const std::string test_case_mortality_event_u_max =
    R"(
@model
start_year 2000
final_year 2002
min_age 1
max_age 5
base_weight_units kgs
age_plus f
time_steps step_one

@categories
format sex
names male female
age_lengths no_age_length*2

@age_length no_age_length
type none
length_weight [type=none]

@time_step step_one
processes ageing recruitment mortality

@ageing ageing
categories *

@recruitment recruitment
type constant
categories *
proportions 0.6 0.4
r0 100000
age 1

@selectivity one
type constant
c 1

@penalty event_penalty
type process
log_scale false
multiplier 1

@mortality mortality
type event
categories male female
years 2000 2001 2002
catches 1000000 50000 1000000
U_max 0.5
selectivities one one
penalty event_penalty
)";

TEST_F(InternalEmptyModel, Processes_Mortality_Event_UMaxCap_Penalty) {
  AddConfigurationLine(test_case_mortality_event_u_max, __FILE__, __LINE__);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male = model_->partition().category("male");
  EXPECT_DOUBLE_EQ(30000, male.data_[0]);
  EXPECT_DOUBLE_EQ(20000, male.data_[1]);
  EXPECT_DOUBLE_EQ(10000, male.data_[2]);
  EXPECT_DOUBLE_EQ(0.0, male.data_[3]);
  EXPECT_DOUBLE_EQ(0.0, male.data_[4]);

  partition::Category& female = model_->partition().category("female");
  EXPECT_DOUBLE_EQ(20000, female.data_[0]);
  EXPECT_DOUBLE_EQ(13333.333333333334, female.data_[1]);
  EXPECT_DOUBLE_EQ(6666.666666666667, female.data_[2]);

  Penalty* penalty = model_->managers()->penalty()->GetPenalty("event_penalty");
  ASSERT_NE(penalty, nullptr);
  EXPECT_DOUBLE_EQ(1712500000000, AS_DOUBLE(penalty->GetScore()));

  Process* process = model_->managers()->process()->GetProcess("mortality");
  ASSERT_NE(process, nullptr);
  ostringstream report;
  process->FillReportCache(report);
  EXPECT_EQ("years: 2000 2001 2002 \nactual_catches: 50000 50000 100000 \nexploitation_rate: 0.5 0.333333 0.5 \n", report.str());
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* TESTMODE */
