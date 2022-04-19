/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 23/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Factory.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Selectivities/Common/AllValues.h"
#include "../Selectivities/Common/AllValuesBounded.h"
#include "../Selectivities/Common/Constant.h"
#include "../Selectivities/Common/DoubleExponential.h"
#include "../Selectivities/Common/DoubleNormal.h"
#include "../Selectivities/Common/CompoundRight.h"
#include "../Selectivities/Common/CompoundLeft.h"
#include "../Selectivities/Common/CompoundMiddle.h"
#include "../Selectivities/Common/CompoundAll.h"
#include "../Selectivities/Common/DoubleNormalPlateau.h"
#include "../Selectivities/Common/DoubleNormalSS3.h"
#include "../Selectivities/Common/Increasing.h"
#include "../Selectivities/Common/InverseLogistic.h"
#include "../Selectivities/Common/KnifeEdge.h"
#include "../Selectivities/Common/Logistic.h"
#include "../Selectivities/Common/LogisticProducing.h"
#include "../Selectivities/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Create the instance of the object as defined by the two parameters
 * object_type and sub_type.
 *
 * @param object_type The type of object to create (e.g., age_size, process)
 * @param sub_type The child type of the object to create (e.g., ageing, schnute)
 * @return shared_ptr to the object
 */
Selectivity* Factory::Create(shared_ptr<Model> model, const string& object_type, const string& sub_type) {
  Selectivity* result = nullptr;

  if (object_type == PARAM_SELECTIVITY || object_type == PARAM_SELECTIVITIES || object_type == PARAM_RELATIVE_M_BY_AGE || object_type == PARAM_LOSS_RATE_SELECTIVITIES) {
    if (sub_type == PARAM_ALL_VALUES)
      result = new AllValues(model);
    else if (sub_type == PARAM_ALL_VALUES_BOUNDED)
      result = new AllValuesBounded(model);
    else if (sub_type == PARAM_CONSTANT)
      result = new Constant(model);
    else if (sub_type == PARAM_COMPOUND_RIGHT)
      result = new CompoundRight(model);
    else if (sub_type == PARAM_COMPOUND_LEFT)
      result = new CompoundLeft(model);
    else if (sub_type == PARAM_COMPOUND_MIDDLE)
      result = new CompoundMiddle(model);
    else if (sub_type == PARAM_COMPOUND_ALL)
      result = new CompoundAll(model);
    else if (sub_type == PARAM_DOUBLE_EXPONENTIAL)
      result = new DoubleExponential(model);
    else if (sub_type == PARAM_DOUBLE_NORMAL)
      result = new DoubleNormal(model);
    else if (sub_type == PARAM_DOUBLE_NORMAL_PLATEAU)
      result = new DoubleNormalPlateau(model);
    else if (sub_type == PARAM_DOUBLE_NORMAL_SS3)
      result = new DoubleNormalSS3(model);
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
      model->managers()->selectivity()->AddObject(result);
  }

  return result;
}

} /* namespace selectivities */
} /* namespace niwa */
