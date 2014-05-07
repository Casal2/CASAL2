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

#include <string>
#include <iostream>
#include <fstream>

#include "Reports/Manager.h"

// Namespaces
namespace isam {

using std::streambuf;
using std::ofstream;
using std::cout;
using std::endl;
using std::ios_base;

/**
 * Default constructor
 */
Report::Report() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", true);
  parameters_.Bind<string>(PARAM_FILE_NAME, &file_name_, "File Name", "");
  parameters_.Bind<bool>(PARAM_OVERWRITE, &overwrite_, "Overwrite file", true);
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
  if (file_name_ != "") {
    string suffix = reports::Manager::Instance().report_suffix();

    bool overwrite = false;
    if (suffix != last_suffix_)
      overwrite = overwrite_;
    last_suffix_ = suffix;

    string file_name = file_name_ + suffix;

    ios_base::openmode mode = ios_base::out;
    if (!overwrite)
      mode = ios_base::app;

    // Try to Open our File
    ofstream file;
    file.open(file_name.c_str(), mode);
    if (!file.is_open())
      LOG_ERROR("Unable to open file: " << file_name);

    file << cache_.str();
    file.close();

  } else {
    cout << cache_.str();
    cout << CONFIG_END_REPORT << "\n" << endl;
    cout.flush();
  }

  ready_for_writing_ = false;
}

} /* namespace isam */

