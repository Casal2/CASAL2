/**
 * @file KnifeEdge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "KnifeEdge.h"

// namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor (using constructor delegation)
 */
KnifeEdge::KnifeEdge()
: KnifeEdge(Model::Instance()) {
}

/**
 * Explicit Constructor
 */
KnifeEdge::KnifeEdge(ModelPtr model)
: Selectivity(model) {

  parameters_.Bind<double>(PARAM_E, &edge_, "Edge");
  parameters_.Bind<double>(PARAM_ALPHA, &alpha_, "Alpha", 1.0);

  RegisterAsEstimable(PARAM_ALPHA, &alpha_);
  RegisterAsEstimable(PARAM_E, &edge_);
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void KnifeEdge::Reset() {
  for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
    Double temp = age * 1.0;
    if (temp >= edge_)
      values_[age] = alpha_;
    else
      values_[age] = 0.0;
  }
}

} /* namespace selectivities */
} /* namespace isam */
