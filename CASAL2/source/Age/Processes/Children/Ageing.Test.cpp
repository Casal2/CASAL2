/**
 * @file Ageing.Test.cpp
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
#include "Ageing.h"

#include <iostream>

#include "Common/Model/Model.h"
#include "Common/Model/Factory.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Partition/Partition.h"
#include "TestResources/TestFixtures/BasicModel.h"

// Namespaces
namespace niwa {
namespace age {
namespace processes {

using std::cout;
using std::endl;
using niwa::testfixtures::BasicModel;

/**
 *
 */
TEST_F(BasicModel, Processes_Ageing) {
  vector<string> categories   = { "immature.male", "immature.female" };

  base::Object* process = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT);
  vector<string> proportions  = { "0.6", "0.4" };
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  process = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, categories, __FILE__, __LINE__);

  // Report
  base::Object* report = model_->factory().CreateObject(PARAM_REPORT, PARAM_DERIVED_QUANTITY);
  ASSERT_NE(report, nullptr);
  report->parameters().Add(PARAM_LABEL, "DQ", __FILE__, __LINE__);
  report->parameters().Add(PARAM_TYPE, "derived_quantity", __FILE__, __LINE__);


  base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  vector<string> processes    = { "recruitment", "ageing" };
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  model_->Start(RunMode::kTesting);

  partition::Category& immature_male   = model_->partition().category("immature.male");
  partition::Category& immature_female = model_->partition().category("immature.female");

  // Verify blank partition
  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;

  /**
   * Do 1 iteration of the model then check the categories to see if
   * the ageing was successful
   */
  model_->FullIteration();

  // Check i = 0
  EXPECT_DOUBLE_EQ(0, immature_male.data_[0]);
  EXPECT_DOUBLE_EQ(0, immature_female.data_[0]);
  // Check from i = 1
  for (unsigned i = 1; i < immature_male.data_.size() && i < 16; ++i)
    EXPECT_DOUBLE_EQ(60000.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 1; i < immature_female.data_.size() && i < 16; ++i)
    EXPECT_DOUBLE_EQ(40000.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
  // Check from i = 16
  for (unsigned i = 16; i < immature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 16; i < immature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */


#endif /* TESTMODE */
