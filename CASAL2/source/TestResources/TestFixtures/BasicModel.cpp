/**
 * @file BasicModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 2/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "BasicModel.h"

#include "../../Model/Models/Age.h"
#include "../../Model/Factory.h"
#include "../../Categories/Categories.h"
#include "../../Logging/Logging.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "../../Utilities/To.h"

// Namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
void BasicModel::SetUp() {
  Base::SetUp();

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(2468);

  /**
   * Add Model Parameters
   */
  model_->set_block_type(PARAM_MODEL);
  model_->parameters().Add(PARAM_TYPE, PARAM_AGE, __FILE__, __LINE__);
  model_->parameters().Add(PARAM_START_YEAR, "1994", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_FINAL_YEAR, "2008", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_MIN_AGE, "1", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_MAX_AGE, "20", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_PLUS_GROUP, "true", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_TIME_STEPS, "step_one", __FILE__, __LINE__);
  model_->parameters().Add(PARAM_BASE_UNITS, "kgs", __FILE__, __LINE__);
  model_->PopulateParameters();

  base::Object* categories = model_->categories();
  categories->set_block_type(PARAM_CATEGORIES);
  categories->parameters().Add(PARAM_FORMAT, "stage.sex", __FILE__, __LINE__);
  categories->parameters().Add(PARAM_NAMES, { "immature.male", "mature.male", "immature.female", "mature.female" }, __FILE__, __LINE__);

  base::Object* object = model_->factory().CreateObject(PARAM_SELECTIVITY, PARAM_CONSTANT);
  object->parameters().Add(PARAM_LABEL, "constant_one", __FILE__, __LINE__);
  object->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  object->parameters().Add(PARAM_C, "1", __FILE__, __LINE__);
}

} /* namespace testfixtures */
} /* namespace niwa */

#endif /* TESTMODE */
