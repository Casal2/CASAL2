/**
 * @file CombinedCategories.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// headers
#include "CombinedCategories.h"

#include "../../../Model/Factory.h"
#include "../../../Model/Models/Age.h"
#include "../../../Partition/Partition.h"
#include "../../../TestResources/TestFixtures/BasicModel.h"
#include "../../../TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace partition {
namespace accessors {
namespace cached {

using niwa::testfixtures::BasicModel;

/**
 *
 */
TEST_F(BasicModel, Accessors_Cached_CombinedCategories) {
  ASSERT_NE(model_, nullptr);

  // Recruitment process
  vector<string> recruitment_categories = {"immature.male", "immature.female"};
  vector<string> proportions            = {"0.6", "0.4"};
  base::Object*  process                = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT, PartitionType::kAge);
  ASSERT_NE(process, nullptr);
  process->parameters().Add(PARAM_LABEL, "recruitment", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, __FILE__, __LINE__);
  process->parameters().Add(PARAM_R0, "100000", __FILE__, __LINE__);
  process->parameters().Add(PARAM_AGE, "1", __FILE__, __LINE__);

  // Report process
  base::Object* report = model_->factory().CreateObject(PARAM_REPORT, PARAM_DERIVED_QUANTITY);
  ASSERT_NE(report, nullptr);
  report->parameters().Add(PARAM_LABEL, "DQ", __FILE__, __LINE__);
  report->parameters().Add(PARAM_TYPE, "derived_quantity", __FILE__, __LINE__);

  // Mortality process
  vector<string> mortality_categories = {"immature.male", "immature.female", "mature.male", "mature.female"};
  process                             = model_->factory().CreateObject(PARAM_MORTALITY, PARAM_CONSTANT_RATE, PartitionType::kAge);
  ASSERT_NE(process, nullptr);
  process->parameters().Add(PARAM_LABEL, "mortality", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TYPE, "constant_rate", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, mortality_categories, __FILE__, __LINE__);
  process->parameters().Add(PARAM_M, "0.065", __FILE__, __LINE__);
  process->parameters().Add(PARAM_RELATIVE_M_BY_AGE, "constant_one", __FILE__, __LINE__);
  process->parameters().Add(PARAM_TIME_STEP_PROPORTIONS, "1.0", __FILE__, __LINE__);

  // Ageing process
  vector<string> ageing_categories = {"immature.male", "immature.female"};
  process                          = model_->factory().CreateObject(PARAM_AGEING, "");
  ASSERT_NE(process, nullptr);
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, ageing_categories, __FILE__, __LINE__);

  // Timestep
  base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  ASSERT_NE(time_step, nullptr);
  vector<string> processes = {"ageing", "recruitment", "mortality"};
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);

  // Run the model
  model_->Start(RunMode::kTesting);
  model_->FullIteration();

  // Build our accessor
  vector<string>              accessor_category_labels = {"immature.male+immature.female", "immature.male", "immature.female"};
  CachedCombinedCategoriesPtr accessor                 = CachedCombinedCategoriesPtr(new CombinedCategories(model_, accessor_category_labels));

  accessor->BuildCache();
  ASSERT_EQ(3u, accessor->Size());

  accessor->BuildCache();
  ASSERT_EQ(3u, accessor->Size());

  auto cache_iter = accessor->Begin();
  ASSERT_EQ(cache_iter->size(), 2u);
  EXPECT_EQ((*cache_iter)[0]->name_, "immature.male");
  EXPECT_EQ((*cache_iter)[1]->name_, "immature.female");

  ++cache_iter;
  ASSERT_EQ(cache_iter->size(), 1u);
  EXPECT_EQ((*cache_iter)[0]->name_, "immature.male");

  ++cache_iter;
  ASSERT_EQ(cache_iter->size(), 1u);
  EXPECT_EQ((*cache_iter)[0]->name_, "immature.female");
}

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
#endif
