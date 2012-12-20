/**
 * @file MortalityEvent.cpp
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
#include "MortalityEvent.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MortalityEvent::MortalityEvent() {

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_YEARS);
  parameters_.RegisterAllowed(PARAM_CATCHES);
  parameters_.RegisterAllowed(PARAM_U_MAX);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
  parameters_.RegisterAllowed(PARAM_PENALTY);
}

/**
 * Validate our Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityEvent::Validate() {

  // Check for Required Parameters
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_YEARS);
  CheckForRequiredParameter(PARAM_CATCHES);
  CheckForRequiredParameter(PARAM_U_MAX);
  CheckForRequiredParameter(PARAM_SELECTIVITIES);

  // Assign Parameters
  label_              = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_     = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  years_              = parameters_.Get(PARAM_YEARS).GetValues<unsigned>();
  catches_            = parameters_.Get(PARAM_CATCHES).GetValues<double>();
  u_max_              = parameters_.Get(PARAM_U_MAX).GetValue<double>();
  selectivity_names_  = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();
  penalty_            = parameters_.Get(PARAM_PENALTY).GetValue<string>();

  // Validate that our number of years and catches vectors are the same size
  if (years_.size() != catches_.size()) {
    Parameter parameter = parameters_.Get(PARAM_CATCHES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of catches provided does not match the number of years provided."
        << " Expected " << years_.size() << " but got " << catches_.size());
  }

  // Validate that the number of selectivities is the same as the number of categories
  if (category_names_.size() != selectivity_names_.size()) {
    Parameter parameter = parameters_.Get(PARAM_SELECTIVITIES);
    LOG_ERROR("At line " << parameter.line_number() << " in file " << parameter.file_name()
        << ": Number of selectivities provided does not match the number of categories provided."
        << " Expected " << category_names_.size() << " but got " << selectivity_names_.size());
  }

}

} /* namespace processes */
} /* namespace isam */
