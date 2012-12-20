/**
 * @file MortalityConstantRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityConstantRate.h"

#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace processes {

/**
 *
 */
MortalityConstantRate::MortalityConstantRate() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_M);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);

}

/**
 * Validate our Mortality Constant Rate process
 *
 * 1. Validate the required parameters
 * 2. Assign the label from the parameters
 * 3. Assign and validate remaining parameters
 */
void MortalityConstantRate::Validate() {

  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_M);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);
  AssignLabelFromParameters();

  // Assign and validate parameters
  category_names_ = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();

  Parameter parameter = parameters_.Get(PARAM_M);
  m_ = parameter.GetValues<double>();
  if (m_.size() != category_names_.size()) {
    LOG_ERROR("At line " << parameter.line_number() << " of file " << parameter.file_name()
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << m_.size());
  }

  parameter = parameters_.Get(PARAM_SELECTIVITIES);
  selectivity_names_ = parameter.GetValues<string>();
  if (selectivity_names_.size() != category_names_.size()) {
    LOG_ERROR("At line " << parameter.line_number() << " of file " << parameter.file_name()
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << selectivity_names_.size());
  }
}

} /* namespace processes */
} /* namespace isam */
