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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Reports/Manager.h"
#include "Common/TimeSteps/Manager.h"

// Namespaces
namespace niwa {

using std::streambuf;
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;
using std::ios_base;

std::mutex Report::lock_;

inline bool DoesFileExist(const string& file_name) {
  LOG_FINEST() << "Checking if file exists: " << file_name;
  ifstream  file(file_name.c_str());
  if (file.fail() || !file.is_open())
    return false;

  file.close();
  return true;
}

/**
 * Default constructor
 */
Report::Report(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the report", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of report", "");
  parameters_.Bind<string>(PARAM_FILE_NAME, &file_name_, "The File Name if you want this report to be in a seperate file", "", "");
  parameters_.Bind<string>(PARAM_WRITE_MODE, &write_mode_, "The write mode", "", PARAM_OVERWRITE)
      ->set_allowed_values({ PARAM_OVERWRITE, PARAM_APPEND, PARAM_INCREMENTAL_SUFFIX });
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
  if (time_step_ != "" && !model_->managers().time_step()->GetTimeStep(time_step_))
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
 * The prepare method is called once before the model is run. It's done
 * post-build in the model and will allow the report to check if
 * the file it wants to write to exists etc.
 */
void Report::Prepare() {
  LOG_FINEST() << "preparing report: " << label_;
  Report::lock_.lock();
  SetUpInternalStates();
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
 * Prepare the report
 */
void Report::PrepareTabular() {
  LOG_FINEST() << "preparing tabular report: " << label_;
  Report::lock_.lock();
  SetUpInternalStates();
  DoPrepareTabular();
  Report::lock_.unlock();
}

/**
 *
 */
void Report::ExecuteTabular() {
  Report::lock_.lock();
  DoExecuteTabular();
  Report::lock_.unlock();
}

/**
 *
 */
void Report::FinaliseTabular() {
  Report::lock_.lock();
  DoFinaliseTabular();
  Report::lock_.unlock();
}

/**
 *
 */
void Report::SetUpInternalStates() {
  /**
   * Figure out the write options. If we're using an incremental suffix
   * we want to loop over the existing files to see what suffix to use.
   */
  if (file_name_ != "" && write_mode_ == PARAM_INCREMENTAL_SUFFIX) {
    LOG_FINEST() << "Finding incremental suffix for file: " << label_;
    if (DoesFileExist(file_name_)) {
      for (unsigned i = 0; i < 1000; ++i) {
        string trial_name = file_name_ + "." + util::ToInline<unsigned, string>(i);
        if (!DoesFileExist(trial_name)) {
          LOG_FINE() << "File name has been changed too " << trial_name << " to match incremental_suffix";
          file_name_ = trial_name;
          break;
        }
      }
    }
  }

  if (write_mode_ == PARAM_APPEND) {
    LOG_FINEST() << "File write mode is append for file: " << label_;
    overwrite_ = false;
  }
}

/**
 * Flush the contents of the cache to the file or stdout/stderr
 */
void Report::FlushCache() {
  Report::lock_.lock();

  /**
   * Are we writing to a file?
   */
  if (file_name_ != "") {
    string suffix = model_->managers().report()->report_suffix();

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

    if (!skip_tags_)
      cache_ << CONFIG_END_REPORT << "\n";

    file << cache_.str();
    file.close();

    first_write_ = false;

  } else {
    cout << cache_.str();
    if (!skip_tags_)
      cout << CONFIG_END_REPORT << "\n";
    cout << endl;
    cout.flush();

    first_write_ = false;
  }

  cache_.clear();
  cache_.str("");
  ready_for_writing_ = false;
  Report::lock_.unlock();
}

} /* namespace niwa */

