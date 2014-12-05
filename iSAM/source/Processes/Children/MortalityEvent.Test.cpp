/**
 * @file MortalityEvent.Test.cpp
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
#include "MortalityEvent.h"

#include <iostream>

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
TEST_F(BasicModel, Processes_Mortality_Event_No_Penalty) {

  // Recruitment process
  vector<string> recruitment_categories   = { "immature.male", "immature.female" };
  vector<string> proportions  = { "0.6", "0.4" };
  niwa::ProcessPtr process = processes::Factory::Create(PARAM_RECRUITMENT, PARAM_CONSTANT);
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  // Mortality process
  vector<string> mortality_categories   = { "immature.male", "immature.female", "mature.male", "mature.female" };
  vector<string> years    = { "1998", "1999", "2000", "2001", "2002", "2003", "2004" };
  vector<string> catches  = { "1000", "2000", "3000", "5000", "8000", "13000", "21000" };
  vector<string> selectivities = { "constant_one", "constant_one", "constant_one", "constant_one" };
  process = processes::Factory::Create(PARAM_MORTALITY, PARAM_EVENT);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "event", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_YEARS, years, __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATCHES, catches, __FILE__, __LINE__);
  process->parameters().Add(PARAM_U_MAX, "0.99", __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, selectivities, __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories   = { "immature.male", "immature.female" };
  process = processes::Factory::Create(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  niwa::base::ObjectPtr time_step = timesteps::Factory::Create();
  vector<string> processes    = { "ageing", "recruitment", "mortality" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  Model::Instance()->Start(RunMode::kTesting);

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

  vector<double> immature_males = { 60000, 60000, 60000, 60000, 58820.224719101127,
                                    58040.751812527917, 57518.450333899993, 57156.243467817498,
                                    56910.233811571517, 56720.216336407662, 56606.775903734859,
                                    56606.775903734859, 56606.775903734859, 56606.775903734859,
                                    56606.775903734859};
  vector<double> immature_females = { 40000, 40000, 40000, 40000, 39213.483146067418,
                                    38693.834541685283, 38345.633555933331, 38104.162311878339,
                                    37940.155874381009, 37813.477557605118, 37737.850602489903,
                                    37737.850602489903, 37737.850602489903, 37737.850602489903,
                                    37737.850602489903 };

  for (unsigned i = 0; i < immature_male.data_.size() && i < 15; ++i) {
    EXPECT_DOUBLE_EQ(immature_males[i], immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
    EXPECT_DOUBLE_EQ(immature_females[i], immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  }

  // Mature should still be 0 because we had no maturation
  for (unsigned i = 0; i < mature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, mature_male.data_[i]) << " where i = " << i << "; age = " << i + mature_male.min_age_;
  for (unsigned i = 0; i < mature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, mature_female.data_[i]) << " where i = " << i << "; age = " << i + mature_female.min_age_;
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
