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
namespace isam {
namespace selectivities {

/**
 * Factory method to create our selectivities
 */
SelectivityPtr Factory::Create(const string& block_type, const string& selectivity_type) {

  SelectivityPtr object;

  if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_ALL_VALUES)
    object = SelectivityPtr(new AllValues());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_ALL_VALUES_BOUNDED)
    object = SelectivityPtr(new AllValuesBounded());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_CONSTANT)
    object = SelectivityPtr(new Constant());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_DOUBLE_EXPONENTIAL)
    object = SelectivityPtr(new DoubleExponential());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_DOUBLE_NORMAL)
    object = SelectivityPtr(new DoubleNormal());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_INCREASING)
    object = SelectivityPtr(new Increasing());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_INVERSE_LOGISTIC)
    object = SelectivityPtr(new InverseLogistic());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_KNIFE_EDGE)
    object = SelectivityPtr(new KnifeEdge());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_LOGISTIC)
    object = SelectivityPtr(new Logistic());
  else if (block_type == PARAM_SELECTIVITY && selectivity_type == PARAM_LOGISTIC_PRODUCING)
    object = SelectivityPtr(new LogisticProducing());

  if (object)
    isam::selectivities::Manager::Instance().AddObject(object);

  return object;

}

} /* namespace selectivities */
} /* namespace isam */
