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
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {

using std::streambuf;
using std::ofstream;
using std::cout;
using std::endl;
using std::ios_base;

std::mutex Report::lock_;

/**
 * Default constructor
 */
Report::Report() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_FILE_NAME, &file_name_, "File Name", "", "");
  parameters_.Bind<bool>(PARAM_OVERWRITE, &overwrite_, "Overwrite file", "", true);
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
 *
 */
void Report::Build() {
  if (time_step_ != "" && !timesteps::Manager::Instance().GetTimeStep(time_step_))
    LOG_ERROR_P(PARAM_TIME_STEP) << ": " << time_step_ << " could not be found. Have you defined it?";

  DoBuild();
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
 *
 */
void Report::Prepare() {
  Report::lock_.lock();
  DoPrepare();
  Report::lock_.unlock();
};

/**
 *
 */
void Report::Execute() {
  Report::lock_.lock();
  DoExecute();
  Report::lock_.unlock();
}

/**
 *
 */
void Report::Finalise() {
  Report::lock_.lock();
  DoFinalise();
  Report::lock_.unlock();
};

/**
 * Flush the contents of the cache to the file or fisk.
 */
void Report::FlushCache() {
  Report::lock_.lock();
  if (file_name_ != "") {
    string suffix = reports::Manager::Instance().report_suffix();

    bool overwrite = false;
    if (first_write_ || suffix != last_suffix_)
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
      LOG_ERROR() << "Unable to open file: " << file_name;

    file << cache_.str();
    file.close();

    first_write_ = false;

  } else {
    cout << cache_.str();
    if (!skip_tags_)
      cout << CONFIG_END_REPORT << "\n";
    cout << endl;
    cout.flush();
  }

  cache_.clear();
  cache_.str("");
  ready_for_writing_ = false;
  Report::lock_.unlock();
}

} /* namespace niwa */

