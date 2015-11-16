
/**
 * @file TransitionCategory.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 4/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include <Processes/Children/TransitionCategory.h>

#include <iostream>

#include "Model/Factory.h"
#include "Processes/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "Selectivities/Factory.h"
#include "TestResources/TestFixtures/BasicModel.h"

// Namespaces
namespace niwa {
namespace processes {

using std::cout;
using std::endl;
using niwa::testfixtures::BasicModel;

/**
 *
 */
TEST_F(BasicModel, Processes_Transition_Category_Constant_One_Selectivity) {

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

  // Maturation Process
  vector<string> from_categories   = { "immature.male", "immature.female" };
  vector<string> to_categories = { "mature.male", "mature.female" };
  vector<string> maturation_proportions = { "0.6", "0.5" };
  process = model_->factory().CreateObject(PARAM_PROCESS, PARAM_TRANSITION_CATEGORY);
  process->parameters().Add(PARAM_LABEL, "maturation", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "category_transition", __FILE__, __LINE__);
  process->parameters().Add(PARAM_FROM, from_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_TO, to_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, "constant_one", __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, maturation_proportions, __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories   = { "immature.male", "immature.female", "mature.male", "mature.female" };
  process = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  vector<string> processes    = { "ageing", "recruitment", "maturation" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  model_->Start(RunMode::kTesting);

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");
  partition::Category& mature_male     = model_->partition().category("mature.male");
  partition::Category& mature_female   = model_->partition().category("mature.female");

  // Verify blank partition
  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  for (unsigned i = 0; i < mature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;

  /**
   * Do 1 iteration of the model then check the categories to see if
   * the maturation was successful
   */
  model_->FullIteration();

  double immature_value = 60000;
  double mature_value   = 0;
  for (unsigned i = 0; i < immature_male.data_.size() && i < 15; ++i) {
    mature_value += immature_value * 0.6;
    immature_value *= 0.4;

    EXPECT_DOUBLE_EQ(immature_value, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_ << "; immature_value = " << immature_value;
    EXPECT_DOUBLE_EQ(mature_value, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_ << "; mature_value = " << mature_value;
  }

  immature_value = 40000;
  mature_value = 0;
  for (unsigned i = 0; i < immature_female.data_.size() && i < 15; ++i) {
    mature_value += immature_value * 0.5;
    immature_value *= 0.5;

    EXPECT_DOUBLE_EQ(immature_value, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_ << "; immature_value = " << immature_value;
    EXPECT_DOUBLE_EQ(mature_value, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_ << "; mature_value = " << mature_value;
  }
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
