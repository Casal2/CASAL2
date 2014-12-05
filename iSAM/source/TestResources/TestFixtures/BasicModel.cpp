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

#include "AgeSizes/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Factory.h"
#include "Selectivities/Manager.h"
#include "SizeWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
void BasicModel::SetUp() {
  Model::Instance(true);

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(2468);

  niwa::base::ObjectPtr object;

  /**
   * Add Model Parameters
   */
  object = Model::Instance();
  object->set_block_type(PARAM_MODEL);
  object->parameters().Add(PARAM_START_YEAR, "1994", __FILE__, __LINE__);
  object->parameters().Add(PARAM_FINAL_YEAR, "2008", __FILE__, __LINE__);
  object->parameters().Add(PARAM_MIN_AGE, "1", __FILE__, __LINE__);
  object->parameters().Add(PARAM_MAX_AGE, "20", __FILE__, __LINE__);
  object->parameters().Add(PARAM_AGE_PLUS, "true", __FILE__, __LINE__);
  object->parameters().Add(PARAM_TIME_STEPS, "step_one", __FILE__, __LINE__);

  object = Categories::Instance();
  vector<string> categories = { "immature.male", "mature.male", "immature.female", "mature.female" };
  object->set_block_type(PARAM_CATEGORIES);
  object->parameters().Add(PARAM_FORMAT, "stage.sex", __FILE__, __LINE__);
  object->parameters().Add(PARAM_NAMES, categories, __FILE__, __LINE__);

  object = selectivities::Factory::Create(PARAM_SELECTIVITY, PARAM_CONSTANT);
  object->parameters().Add(PARAM_LABEL, "constant_one", __FILE__, __LINE__);
  object->parameters().Add(PARAM_TYPE, "constant", __FILE__, __LINE__);
  object->parameters().Add(PARAM_C, "1", __FILE__, __LINE__);
}

} /* namespace testfixtures */
} /* namespace niwa */

#endif /* TESTMODE */
