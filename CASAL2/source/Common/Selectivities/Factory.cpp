/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 23/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Selectivities/Manager.h"
#include "Common/Selectivities/Children/AllValues.h"
#include "Common/Selectivities/Children/AllValuesBounded.h"
#include "Common/Selectivities/Children/Constant.h"
#include "Common/Selectivities/Children/DoubleExponential.h"
#include "Common/Selectivities/Children/DoubleNormal.h"
#include "Common/Selectivities/Children/Increasing.h"
#include "Common/Selectivities/Children/InverseLogistic.h"
#include "Common/Selectivities/Children/KnifeEdge.h"
#include "Common/Selectivities/Children/Logistic.h"
#include "Common/Selectivities/Children/LogisticProducing.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Create the instance of our object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g age_size, process)
 * @param sub_type The child type of the object to create (e.g ageing, schnute)
 * @return shared_ptr to the object we've created
 */
Selectivity* Factory::Create(Model* model, const string& object_type, const string& sub_type) {
  Selectivity* result = nullptr;

  if (object_type == PARAM_SELECTIVITY || object_type == PARAM_SELECTIVITIES) {
    if (sub_type == PARAM_ALL_VALUES)
      result = new AllValues(model);
    else if (sub_type == PARAM_ALL_VALUES_BOUNDED)
      result = new AllValuesBounded(model);
    else if (sub_type == PARAM_CONSTANT)
      result = new Constant(model);
    else if (sub_type == PARAM_DOUBLE_EXPONENTIAL)
      result = new DoubleExponential(model);
    else if (sub_type == PARAM_DOUBLE_NORMAL)
      result = new DoubleNormal(model);
    else if (sub_type == PARAM_INCREASING)
      result = new Increasing(model);
    else if (sub_type == PARAM_INVERSE_LOGISTIC)
      result = new InverseLogistic(model);
    else if (sub_type == PARAM_KNIFE_EDGE)
      result = new KnifeEdge(model);
    else if (sub_type == PARAM_LOGISTIC)
      result = new Logistic(model);
    else if (sub_type == PARAM_LOGISTIC_PRODUCING)
      result = new LogisticProducing(model);

    if (result)
      model->managers().selectivity()->AddObject(result);
  }

  return result;
}

} /* namespace selectivities */
} /* namespace niwa */
