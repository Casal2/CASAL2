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

#include "Selectivities/Manager.h"
#include "Selectivities/Children/AllValues.h"
#include "Selectivities/Children/AllValuesBounded.h"
#include "Selectivities/Children/Constant.h"
#include "Selectivities/Children/DoubleExponential.h"
#include "Selectivities/Children/DoubleNormal.h"
#include "Selectivities/Children/Increasing.h"
#include "Selectivities/Children/InverseLogistic.h"
#include "Selectivities/Children/KnifeEdge.h"
#include "Selectivities/Children/Logistic.h"
#include "Selectivities/Children/LogisticProducing.h"

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
Selectivity* Factory::Create(const string& object_type, const string& sub_type) {
  Selectivity* result = nullptr;

  if (object_type == PARAM_SELECTIVITY || object_type == PARAM_SELECTIVITIES) {
    if (sub_type == PARAM_ALL_VALUES)
      result = new AllValues();
    else if (sub_type == PARAM_ALL_VALUES_BOUNDED)
      result = new AllValuesBounded();
    else if (sub_type == PARAM_CONSTANT)
      result = new Constant();
    else if (sub_type == PARAM_DOUBLE_EXPONENTIAL)
      result = new DoubleExponential();
    else if (sub_type == PARAM_DOUBLE_NORMAL)
      result = new DoubleNormal();
    else if (sub_type == PARAM_INCREASING)
      result = new Increasing();
    else if (sub_type == PARAM_INVERSE_LOGISTIC)
      result = new InverseLogistic();
    else if (sub_type == PARAM_KNIFE_EDGE)
      result = new KnifeEdge();
    else if (sub_type == PARAM_LOGISTIC)
      result = new Logistic();
    else if (sub_type == PARAM_LOGISTIC_PRODUCING)
      result = new LogisticProducing();

    if (result)
      niwa::selectivities::Manager::Instance().AddObject(result);
  }

  return result;
}

} /* namespace selectivities */
} /* namespace niwa */
