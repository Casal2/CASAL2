/**
 * @file Report.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Report.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Report::Report() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_TIME_STEP);
  parameters_.RegisterAllowed(PARAM_YEAR);
}

/**
 * Validate the generic parameters ensuring
 * we cannot specify things like time step and year
 * when the report is not running in the execute phase.
 */
void Report::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);

  label_ = parameters_.Get(PARAM_LABEL).GetValue<string>();

  if (model_state_ == State::kExecute) {
    CheckForRequiredParameter(PARAM_TIME_STEP);
    CheckForRequiredParameter(PARAM_YEAR);

    time_step_ = parameters_.Get(PARAM_TIME_STEP).GetValue<string>();
    years_     = parameters_.Get(PARAM_YEAR).GetValues<unsigned>();
  }
}

/**
 * Check to see if the report has
 * the current year defined as a year
 * when it's suppose to run
 *
 * @param year The year to check
 * @return True if present, false otherwise
 */
bool Report::HasYear(unsigned year) {
  return (std::find(years_.begin(), years_.end(), year) != years_.end());
}

} /* namespace isam */
