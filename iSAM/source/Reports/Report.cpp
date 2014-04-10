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
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", true);
}

/**
 * Validate the generic parameters ensuring
 * we cannot specify things like time step and year
 * when the report is not running in the execute phase.
 */
void Report::Validate() {
  parameters_.Populate();
  DoValidate();
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

/**
 * Flush the contents of the cache to the file or fisk.
 */
void Report::FlushCache() {
  if (cache_.str() != "") {
    cout << cache_.str();
    cout << CONFIG_END_REPORT << "\n" << endl;
    cout.flush();
  }
}

} /* namespace isam */

