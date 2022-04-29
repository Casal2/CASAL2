/**
 * @file KnifeEdge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "KnifeEdge.h"

#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Default Constructor
 */
KnifeEdge::KnifeEdge(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_E, &edge_, "The edge value", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "The maximum value of the selectivity", "", 1.0);

  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
  RegisterAsAddressable(PARAM_E, &edge_);
  allowed_length_based_in_age_based_model_ = true;

}

/**
 * The core function
 */
Double KnifeEdge::get_value(Double value) {
  if (value >= edge_)
    return alpha_;
  else
    return 0.0;
  return 1.0;
}

/**
 * The core function
 */
Double KnifeEdge::get_value(unsigned value) {
  if (value >= edge_)
    return alpha_;
  else
    return 0.0;
  return 1.0;
}

} /* namespace selectivities */
} /* namespace niwa */
