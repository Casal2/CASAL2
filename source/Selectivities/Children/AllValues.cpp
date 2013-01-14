/**
 * @file AllValues.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "AllValues.h"
#include "Model/Model.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Default constructor
 */
AllValues::AllValues() {

  parameters_.RegisterAllowed(PARAM_V);
}

/**
 * Validate the parameters for this selectivity
 */
void AllValues::Validate() {
  LOG_TRACE();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_V);

  // Vs should be the same length as the world age spread
  v_ = parameters_.Get(PARAM_V).GetValues<double>();

  ModelPtr model = Model::Instance();
  if (v_.size() != model->age_spread()) {
    Parameter parameter = parameters_.Get(PARAM_V);
    LOG_ERROR("At line " << parameter.line_number() << " of file " << parameter.file_name()
        << ": Number of 'v' values supplied is not the same as the model age spread.\n"
        << "Expected: " << model->age_spread() << " but got " << v_.size());
  }

  unsigned min_age = model->min_age();
  for (unsigned i = 0; i < v_.size(); ++i) {
    values_[min_age + i] = v_[i];
    // TODO: Register each V as an estimable
    // RegisterEstimable(values_[min_age + i]
  }
}

/**
 *
 */
void AllValues::Reset() {

}

} /* namespace selectivities */
} /* namespace isam */
