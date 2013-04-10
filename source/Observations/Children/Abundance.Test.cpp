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
#include "Abundance.h"

#include <iostream>

#include "Catchabilities/Factory.h"
#include "Observations/Factory.h"
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
TEST_F(BasicModel, Observation_Abundance) {

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

  // Mortality process
  vector<string> mortality_categories   = { "immature.male", "immature.female", "mature.male", "mature.female" };
  process = processes::Factory::Create(PARAM_MORTALITY, PARAM_CONSTANT_RATE);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant_rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_M, "0.065", __FILE__, __LINE__);
  process->parameters().Add(PARAM_SELECTIVITIES, "constant_one", __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories   = { "immature.male", "immature.female" };
  process = processes::Factory::Create(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  isam::base::ObjectPtr time_step = timesteps::Factory::Create();
  vector<string> processes    = { "ageing", "recruitment", "mortality" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  // Catchability
  isam::CatchabilityPtr catchability = catchabilities::Factory::Create();
  catchability->parameters().Add(PARAM_LABEL, "catchability", __FILE__, __LINE__);
  catchability->parameters().Add(PARAM_Q, "0.000153139", __FILE__, __LINE__);

  // Observation
  vector<string> observation_categories = { "immature.male", "immature.female" };
  vector<string> obs = { "all", "22.50" };
  vector<string> error_values = { "all", "0.2" };
  vector<string> selectivities = { "constant_one", "constant_one" };
  isam::ObservationPtr observation = observations::Factory::Create(PARAM_OBSERVATION, PARAM_ABUNDANCE);
  observation->parameters().Add(PARAM_LABEL, "abundance", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_TYPE, "abundance", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_CATCHABILITY, "catchability", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_YEAR, "2008", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_TIME_STEP, "step_one", __FILE__, __LINE__);
  observation->parameters().Add(PARAM_CATEGORIES, observation_categories, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_SELECTIVITIES, selectivities, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_OBS, obs, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_ERROR_VALUE, error_values, __FILE__, __LINE__);
  observation->parameters().Add(PARAM_LIKELIHOOD, "log_normal", __FILE__, __LINE__);

  Model::Instance()->Start();
  Model::Instance()->FullIteration();

  const vector<obs::Comparison>& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  EXPECT_EQ("all", comparisons[0].key_);
  EXPECT_DOUBLE_EQ(0.2, comparisons[0].error_value_);
  EXPECT_DOUBLE_EQ(142.01537476494462, comparisons[0].expected_);
  EXPECT_DOUBLE_EQ(22.5, comparisons[0].observed_);
  EXPECT_DOUBLE_EQ(40.738892086047329, comparisons[0].score_);
}

} /* namespace processes */
} /* namespace isam */


#endif /* TESTMODE */
