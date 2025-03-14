/**
 * @file MortalityDiseaseRate.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/14
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TestResources/TestFixtures/BasicModel.h"
#include "TimeSteps/Manager.h"

namespace niwa::processes::age {
using niwa::testfixtures::BasicModel;
using std::cout;
using std::endl;

/**
 *
 */
TEST_F(BasicModel, Processes_Mortality_Disease_Rate) {
  this->add_process_ageing(__FILE__, __LINE__);
  this->add_process_recruitment_constant(__FILE__, __LINE__);
  this->add_selectivity_logistic(__FILE__, __LINE__);
  this->add_time_step(__FILE__, __LINE__, {"ageing", "recruitment", "mortality"});

  // Mortality process
  vector<string> mortality_categories = {"immature.male", "immature.female"};
  base::Object*  process              = model_->factory().CreateObject(PARAM_PROCESS, PARAM_MORTALITY_DISEASE_RATE);
  vector<string> year_effcts          = {"0.025", "0.035", "0.1", "0.25", "0.001", "0.5", "0.75"};
  vector<string> years                = {"2008", "2007", "2006", "2005", "2004", "2003", "2002"};
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "mortality_disease_rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_DISEASE_MORTALITY_RATE, "0.035", __FILE__, __LINE__);
  process->parameters().Add(PARAM_YEAR_EFFECTS, year_effcts, __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, "constant_one", __FILE__, __LINE__);
  process->parameters().Add(PARAM_YEARS, years, __FILE__, __LINE__);

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

  // Run the model
  model_->FullIteration();

  // Validate our results
  double immature_male_value   = 60000;
  double immature_female_value = 40000;

  vector<double> expected_values = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  vector<double> effect_values   = {0.025, 0.035, 0.1, 0.25, 0.001, 0.5, 0.75, 0.0, 0.0, 0.0};

  for (unsigned i = 0; i < expected_values.size() && i < 15; ++i) {
    immature_male_value -= immature_male_value - (immature_male_value * exp(-1 * 0.035 * effect_values[i]));
    immature_female_value -= immature_female_value - (immature_female_value * exp(-1 * 0.035 * effect_values[i]));

    EXPECT_DOUBLE_EQ(immature_male_value, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
    EXPECT_DOUBLE_EQ(immature_female_value, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  }

  // Mature should still be 0 because we had no maturation
  for (unsigned i = 0; i < mature_male.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i) EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;
}

}  // namespace niwa::processes::age
#endif  // TESTMODE