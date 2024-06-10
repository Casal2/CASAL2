/**
 * @file BasicModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 2/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "BasicModel.h"

#include "../../Categories/Categories.h"
#include "../../Logging/Logging.h"
#include "../../Model/Factory.h"
#include "../../Model/Models/Age.h"
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
  if (!model_)
    LOG_CODE_ERROR() << "!model";
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
  if (categories == nullptr)
    LOG_CODE_ERROR() << "categories == nullptr";
  categories->set_block_type(PARAM_CATEGORIES);
  categories->parameters().Add(PARAM_FORMAT, "stage.sex", __FILE__, __LINE__);
  categories->parameters().Add(PARAM_NAMES, {"immature.male", "mature.male", "immature.female", "mature.female"}, __FILE__, __LINE__);
  categories->parameters().Add(PARAM_AGE_LENGTHS, {"no_age_length","no_age_length","no_age_length","no_age_length"}, __FILE__, __LINE__);
  
  base::Object* age_length_object = model_->factory().CreateObject(PARAM_AGE_LENGTH, PARAM_NONE);
  if (age_length_object == nullptr)
    LOG_CODE_ERROR() << "age_length_object == nullptr";
  age_length_object->parameters().Add(PARAM_LABEL, "no_age_length", __FILE__, __LINE__);
  age_length_object->parameters().Add(PARAM_TYPE, PARAM_NONE, __FILE__, __LINE__);
  age_length_object->parameters().Add(PARAM_LENGTH_WEIGHT, "no_length_weight", __FILE__, __LINE__);

  base::Object* length_weight_object = model_->factory().CreateObject(PARAM_LENGTH_WEIGHT, PARAM_NONE);
  if (length_weight_object == nullptr)
    LOG_CODE_ERROR() << "age_length_object == nullptr";
  length_weight_object->parameters().Add(PARAM_LABEL, "no_length_weight", __FILE__, __LINE__);
  length_weight_object->parameters().Add(PARAM_TYPE, PARAM_NONE, __FILE__, __LINE__);

  base::Object* object = model_->factory().CreateObject(PARAM_SELECTIVITY, PARAM_CONSTANT);
  if (object == nullptr)
    LOG_CODE_ERROR() << "object == nullptr";
  object->parameters().Add(PARAM_LABEL, "constant_one", __FILE__, __LINE__);
  object->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  object->parameters().Add(PARAM_C, "1", __FILE__, __LINE__);
}

} /* namespace testfixtures */
} /* namespace niwa */

#endif /* TESTMODE */
