/**
 * @file RecruitmentBevertonHolt.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "RecruitmentBevertonHolt.h"

#include <iostream>

#include "Processes/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// unit tests configuration headers
#include "TestResources/ConfigurationFiles/Processes/BevertonHolt.h"

// Namespaces
namespace isam {
namespace processes {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment) {

  AddConfigurationLine(basic_beverton_holt, __FILE__, __LINE__);
//
//
//  isam::ProcessPtr process = processes::Factory::Create(PARAM_RECRUITMENT, PARAM_BEVERTON_HOLT);
//  process->parameters().Add(PARAM_LABEL, PARAM_RECRUITMENT, __FILE__, __LINE__);
//  process->parameters().Add(PARAM_TYPE, PARAM_BEVERTON_HOLT, __FILE__, __LINE__);
//  process->parameters().Add(PARAM_CATEGORIES, "immature", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_PROPORTIONS, "1.0", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_R0, "100", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_SSB, "ssb", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_B0, "initialisation_phase_one", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_SSB_OFFSET, "1", __FILE__, __LINE__);
//  process->parameters().Add(PARAM_STANDARDISE_YCS_YEARS, "1994-2006"

//  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);
//
//  isam::base::ObjectPtr time_step = timesteps::Factory::Create();
//  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
//  time_step->parameters().Add(PARAM_PROCESSES, "recruitment", __FILE__, __LINE__);
//
//  Model::Instance()->Start();
//
//  partition::Category& immature_male   = Partition::Instance().category("immature.male");
//  partition::Category& immature_female = Partition::Instance().category("immature.female");
//
//  // Verify blank partition
//  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
//  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
//
//
//  /**
//   * Do 1 iteration of the model then check the categories to see if
//   * the recruitment was successful
//   */
//  Model::Instance()->FullIteration();
//
//  EXPECT_DOUBLE_EQ(900000, immature_male.data_[0]);
//  EXPECT_DOUBLE_EQ(600000, immature_female.data_[0]);
//
//  for (unsigned i = 1; i < immature_male.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
//  for (unsigned i = 1; i < immature_female.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
}

} /* namespace processes */
} /* namespace isam */


#endif /* TESTMODE */
