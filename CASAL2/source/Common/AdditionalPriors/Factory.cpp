/**
 * @file Factory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/AdditionalPriors/Manager.h"
#include "Common/AdditionalPriors/Children/Beta.h"
#include "Common/AdditionalPriors/Children/ElementDifference.h"
#include "Common/AdditionalPriors/Children/LogNormal.h"
#include "Common/AdditionalPriors/Children/UniformLog.h"
#include "Common/AdditionalPriors/Children/VectorAverage.h"
#include "Common/AdditionalPriors/Children/VectorSmoothing.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Create an additional prior
 *
 * @param object_type The type of object
 * @param sub_type The sub type (e.g beta)
 * @return smart_ptr of an additional prior
 */
AdditionalPrior* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
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
  }

  if (object)
    model->managers().additional_prior()->AddObject(object);

  return object;
}

} /* namespace additionalpriors */
} /* namespace niwa */
