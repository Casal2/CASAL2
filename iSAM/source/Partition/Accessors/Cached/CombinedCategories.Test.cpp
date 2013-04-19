/**
 * @file CombinedCategories.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// headers
#include "CombinedCategories.h"

#include "Catchabilities/Factory.h"
#include "Observations/Factory.h"
#include "Processes/Factory.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "Selectivities/Factory.h"
#include "TestResources/TestFixtures/BasicModel.h"

// namespaces
namespace isam {
namespace partition {
namespace accessors {
namespace cached {

using isam::testfixtures::BasicModel;

/**
 *
 */
TEST_F(BasicModel, Accessors_Cached_CombinedCategories) {

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

  // Run the model
  Model::Instance()->Start();
  Model::Instance()->FullIteration();

  Partition::Instance().category("immature.male").years_.push_back(2009);
  Partition::Instance().category("immature.female").years_.push_back(2009);

  // Build our accessor
  vector<string> accessor_category_labels = { "immature.male+immature.female", "immature.male", "immature.female" };
  CachedCombinedCategoriesPtr accessor = CachedCombinedCategoriesPtr(new CombinedCategories(accessor_category_labels));

  accessor->BuildCache();
  ASSERT_EQ(3u, accessor->Size());

//  auto cache_iter = accessor->Begin();
//  EXPECT_EQ("immature.male+immature.female", cache_iter->first);
//  ASSERT_EQ(2u, cache_iter->second.size());
//
//  EXPECT_EQ("immature.male", cache_iter->second[0].name_);
//
//  ++cache_iter;
//  EXPECT_EQ("immature.male", cache_iter->first);
//
//
//  ++cache_iter;
//  EXPECT_EQ("immature.female", cache_iter->first);
}


} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
#endif
