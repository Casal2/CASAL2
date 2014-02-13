/**
 * @file Abundance.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Abundance.h"

#include <iostream>

#include "DerivedQuantities/Factory.h"
#include "Processes/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/ConfigurationFiles/AgeSizes.h"
#include "TestResources/ConfigurationFiles/Categories.h"
#include "TestResources/ConfigurationFiles/Model.h"
#include "TestResources/ConfigurationFiles/SizeWeights.h"
#include "TestResources/ConfigurationFiles/Processes/Ageing.h"
#include "TestResources/ConfigurationFiles/Processes/MortalityConstantRate.h"
#include "TestResources/ConfigurationFiles/Processes/RecruitmentConstant.h"
#include "TestResources/ConfigurationFiles/Selectivities/Constant.h"

// Namespaces
namespace isam {
namespace derivedquantities {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, DerivedQuantities_Abundance) {

  // load the snippets for our configuration
  AddConfigurationLine(model_no_init_phases, __FILE__, __LINE__);
  AddConfigurationLine(categories_no_sex_with_age_size, __FILE__, __LINE__);
  AddConfigurationLine(recruitment_constant_no_sex, __FILE__, __LINE__);
  AddConfigurationLine(processes_ageing_no_sex, __FILE__, __LINE__);
  AddConfigurationLine(age_sizes_von_bert, __FILE__, __LINE__);
  AddConfigurationLine(size_weights_basic, __FILE__, __LINE__);
  AddConfigurationLine(selectivities_constant_one, __FILE__, __LINE__);
  AddConfigurationLine("@time_step time_step_one", __FILE__, __LINE__);
  AddConfigurationLine("processes ageing recruitment_constant", __FILE__, __LINE__);

  LoadConfiguration();

  // build our derived quantity
  vector<string> categories = { "immature", "mature", "spawning" };
  vector<string> selectivities = { "one", "one", "one" };
  DerivedQuantityPtr dq = derivedquantities::Factory::Create(PARAM_DERIVED_QUANTITY, PARAM_ABUNDANCE);
  dq->parameters().Add(PARAM_LABEL, "derived_quantity_biomass", __FILE__, __LINE__);
  dq->parameters().Add(PARAM_TYPE, PARAM_ABUNDANCE, __FILE__, __LINE__);
  dq->parameters().Add(PARAM_TIME_STEP, "time_step_one", __FILE__, __LINE__);
  dq->parameters().Add(PARAM_CATEGORIES, categories, __FILE__, __LINE__);
  dq->parameters().Add(PARAM_SELECTIVITIES, selectivities, __FILE__, __LINE__);

  // run the model
  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  // check the results
  for (unsigned i = 0; i < 15; ++i) {
    unsigned year = 1994 + i;
    double value = (i + 1) * 997386.0;

    EXPECT_DOUBLE_EQ(value, dq->GetValue(year)) << " for year " << year << " and value " << value;
  }

}

} /* namespace derivedquantities */
} /* namespace isam */

#endif
