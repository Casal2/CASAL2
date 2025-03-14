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
namespace niwa::testfixtures {

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
  categories->parameters().Add(PARAM_AGE_LENGTHS, {"no_age_length", "no_age_length", "no_age_length", "no_age_length"}, __FILE__, __LINE__);

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

/**
 * Add a simple process.ageing
 */
void BasicModel::add_process_ageing(const string& file, const unsigned& line, const vector<string> categories) {
  base::Object* process = model_->factory().CreateObject(PARAM_AGEING, "");
  process->parameters().Add(PARAM_LABEL, "ageing", __FILE__, __LINE__);
  process->parameters().Add(PARAM_CATEGORIES, categories, __FILE__, __LINE__);
}

/**
 * Add a simple process.recruitment_constant
 */
void BasicModel::add_process_recruitment_constant(const string& file, const unsigned& line) {
  vector<string> recruitment_categories = {"immature.male", "immature.female"};
  vector<string> proportions            = {"0.6", "0.4"};

  base::Object* process = model_->factory().CreateObject(PARAM_RECRUITMENT, PARAM_CONSTANT);
  process->parameters().Add(PARAM_LABEL, "recruitment", file, line);
  process->parameters().Add(PARAM_TYPE, "constant", file, line);
  process->parameters().Add(PARAM_CATEGORIES, recruitment_categories, file, line);
  process->parameters().Add(PARAM_PROPORTIONS, proportions, file, line);
  process->parameters().Add(PARAM_R0, "100000", file, line);
  process->parameters().Add(PARAM_AGE, "1", file, line);
}

/**
 * Add a simple selectivity.logistic
 */
void BasicModel::add_selectivity_logistic(const string& file, const unsigned& line) {
  base::Object* selectivity = model_->factory().CreateObject(PARAM_SELECTIVITY, PARAM_LOGISTIC);
  selectivity->parameters().Add(PARAM_LABEL, "logistic", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_TYPE, "logistic", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_A50, "8", __FILE__, __LINE__);
  selectivity->parameters().Add(PARAM_ATO95, "3", __FILE__, __LINE__);
}

/**
 *
 */
void BasicModel::add_time_step(const string& file, const unsigned& line, vector<string> processes) {
  base::Object* time_step = model_->factory().CreateObject(PARAM_TIME_STEP, "");
  time_step->parameters().Add(PARAM_LABEL, "step_one", __FILE__, __LINE__);
  time_step->parameters().Add(PARAM_PROCESSES, processes, __FILE__, __LINE__);
}

}  // namespace niwa::testfixtures

#endif /* TESTMODE */
