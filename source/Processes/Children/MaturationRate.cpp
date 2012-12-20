/**
 * @file MaturationRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MaturationRate.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MaturationRate::MaturationRate() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_FROM);
  parameters_.RegisterAllowed(PARAM_TO);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
}

/**
 * Validate our Maturation Rate process
 *
 * 1. Check for the required parameters
 * 2. Assign variables from our parameters
 */
void MaturationRate::Validate() {
  // Check for required parameters
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_FROM);
  CheckForRequiredParameter(PARAM_TO);
  CheckForRequiredParameter(PARAM_PROPORTIONS);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  // Assign local variables
  label_                = parameters_.Get(PARAM_LABEL).GetValue<string>();
  from_category_names_  = parameters_.Get(PARAM_FROM).GetValues<string>();
  to_category_names_    = parameters_.Get(PARAM_TO).GetValues<string>();
  proportions_          = parameters_.Get(PARAM_PROPORTIONS).GetValues<double>();
  selectivity_names_    = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  // Validate the from and to vectors are the same size
  if (from_category_names_.size() != to_category_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_TO);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of to categories provided does not match the number of from categories provided."
        << " Expected " << from_category_names_.size() << " but got " << to_category_names_.size());
  }

  // Validate the to category and proportions vectors are the same size
  if (to_category_names_.size() != proportions_.size()) {
    Parameter parameter = parameters_.Get(PARAM_PROPORTIONS);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of proportions provided does not match the number of 'to' categories provided."
        << " Expected " << to_category_names_.size() << " but got " << proportions_.size());
  }

  // Validate the number of selectivities matches the number of proportions
  if (proportions_.size() != selectivity_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_SELECTIVITIES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of selectivities provided does not match the number of proportions provided."
        << " Expected " << proportions_.size() << " but got " << selectivity_names_.size());
  }
}

} /* namespace processes */
} /* namespace isam */
