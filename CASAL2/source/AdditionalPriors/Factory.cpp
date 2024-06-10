/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Factory.h"

#include "../AdditionalPriors/Common/Beta.h"
#include "../AdditionalPriors/Common/ElementDifference.h"
#include "../AdditionalPriors/Common/LogNormal.h"
#include "../AdditionalPriors/Common/UniformLog.h"
#include "../AdditionalPriors/Common/VectorAverage.h"
#include "../AdditionalPriors/Common/VectorSmoothing.h"
#include "../AdditionalPriors/Common/Ratio.h"
#include "../AdditionalPriors/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Create an additional prior
 *
 * @param object_type The type of object
 * @param sub_type The subtype of the object (e.g., beta)
 * @return smart_ptr of an additional prior
 */
AdditionalPrior* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  AdditionalPrior* object = nullptr;

  if (object_type == PARAM_ADDITIONAL_PRIOR) {
    if (sub_type == PARAM_BETA)
      object = new Beta(model);
    else if (sub_type == PARAM_ELEMENT_DIFFERENCE)
      object = new ElementDifference(model);
    else if (sub_type == PARAM_VECTOR_SMOOTHING)
      object = new VectorSmoothing(model);
    else if (sub_type == PARAM_VECTOR_AVERAGE)
      object = new VectorAverage(model);
    else if (sub_type == PARAM_LOGNORMAL)
      object = new LogNormal(model);
    else if (sub_type == PARAM_UNIFORM_LOG)
      object = new UniformLog(model);
    else if (sub_type == PARAM_RATIO)
      object = new Ratio(model);
  }

  if (object)
    model->managers()->additional_prior()->AddObject(object);

  return object;
}

} /* namespace additionalpriors */
} /* namespace niwa */
