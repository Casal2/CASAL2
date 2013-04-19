/**
 * @file MaturationRate.Test.cpp
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
#include "MaturationRate.h"

#include <iostream>

#include "Processes/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "Selectivities/Factory.h"
#include "TestResources/TestFixtures/BasicModel.h"

// Namespaces
namespace isam {
namespace processes {

using std::cout;
using std::endl;
using isam::testfixtures::BasicModel;

/**
 *
 */
TEST_F(BasicModel, Processes_Maturation_Rate_Constant_One_Selectivity) {

  // Recruitment process
  vector<string> recruitment_categories   = { "immature.male", "immature.female" };
  vector<string> proportions  = { "0.6", "0.4" };
  isam::ProcessPtr process = processes::Factory::Create(PARAM_RECRUITMENT, PARAM_CONSTANT);
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
  process = processes::Factory::Create(PARAM_MATURATION, PARAM_RATE);
  process->parameters().Add(PARAM_LABEL, "maturation", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_FROM, from_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_TO, to_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, "constant_one", __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, maturation_proportions, __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories   = { "immature.male", "immature.female", "mature.male", "mature.female" };
  process = processes::Factory::Create(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  isam::base::ObjectPtr time_step = timesteps::Factory::Create();
  vector<string> processes    = { "ageing", "recruitment", "maturation" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  Model::Instance()->Start();

  partition::Category& immature_male   = Partition::Instance().category("immature.male");
  partition::Category& immature_female = Partition::Instance().category("immature.female");
  partition::Category& mature_male     = Partition::Instance().category("mature.male");
  partition::Category& mature_female   = Partition::Instance().category("mature.female");

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
  Model::Instance()->FullIteration();

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
} /* namespace isam */


#endif /* TESTMODE */
