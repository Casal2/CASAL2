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
#include "Partition/Accessors/All.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/ConfigurationFiles/AgeSizes.h"
#include "TestResources/ConfigurationFiles/Categories.h"
#include "TestResources/ConfigurationFiles/InitialisationPhases.h"
#include "TestResources/ConfigurationFiles/Model.h"
#include "TestResources/ConfigurationFiles/SizeWeights.h"
#include "TestResources/ConfigurationFiles/DerivedQuantities/Abundance.h"
#include "TestResources/ConfigurationFiles/Processes/Ageing.h"
#include "TestResources/ConfigurationFiles/Processes/MaturationRate.h"
#include "TestResources/ConfigurationFiles/Processes/MortalityConstantRate.h"
#include "TestResources/ConfigurationFiles/Processes/RecruitmentBevertonHolt.h"
#include "TestResources/ConfigurationFiles/Selectivities/Constant.h"
#include "TestResources/ConfigurationFiles/Selectivities/LogisticProducing.h"

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

  // load the snippets for our configuration
  AddConfigurationLine(model_one_init_phase, __FILE__, __LINE__);
  AddConfigurationLine(age_sizes_von_bert_no_age_size, __FILE__, __LINE__);
  AddConfigurationLine(categories_no_sex_with_age_size, __FILE__, __LINE__);
  AddConfigurationLine(derived_quantity_abudance_ssb, __FILE__, __LINE__);
  AddConfigurationLine(initialisation_phases_one, __FILE__, __LINE__);
  AddConfigurationLine(processes_ageing_no_sex_no_spawning, __FILE__, __LINE__);
  AddConfigurationLine(processes_maturation_rate_immature_mature, __FILE__, __LINE__);
  AddConfigurationLine(processes_mortality_constant_rate_no_sex, __FILE__, __LINE__);
  AddConfigurationLine(processes_recruitment_beverton_holt, __FILE__, __LINE__);
  AddConfigurationLine(selectivities_constant_one, __FILE__, __LINE__);
  AddConfigurationLine(selectivities_logistic_producing_maturation, __FILE__, __LINE__);
  AddConfigurationLine(size_weights_none, __FILE__, __LINE__);
  AddConfigurationLine("@time_step time_step_one; processes ageing maturation mortality", __FILE__, __LINE__);

  LoadConfiguration();

  // run the model
  ModelPtr model = Model::Instance();
  model->Start(RunMode::kTesting);

  partition::Category& immature_male   = Partition::Instance().category("immature");
  partition::Category& immature_female = Partition::Instance().category("mature");

  // Verify blank partition
  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
}

} /* namespace processes */
} /* namespace isam */


#endif /* TESTMODE */
