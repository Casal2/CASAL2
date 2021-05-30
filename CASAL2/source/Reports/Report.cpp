/**
 * @file Report.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Report.h"

#include <fstream>
#include <iostream>
#include <string>

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Reports/Manager.h"
#include "../TimeSteps/Manager.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;
using std::ifstream;
using std::ios_base;
using std::ofstream;
using std::streambuf;

std::mutex Report::lock_;

/**
 * Check if file exists
 *
 * @param file_name The name of the file
 * @return true if yes, false if now
 */
inline bool DoesFileExist(const string& file_name) {
  LOG_FINEST() << "Checking if file exists: " << file_name;
  ifstream file(file_name.c_str());
  if (file.fail() || !file.is_open())
    return false;

  file.close();
  return true;
}

/**
 * Default constructor
 */
Report::Report() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for the report", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of report", "");
  parameters_.Bind<string>(PARAM_FILE_NAME, &file_name_, "The filename for this report to be in a separate file", "", "");
  parameters_.Bind<string>(PARAM_WRITE_MODE, &write_mode_, "The write mode", "", PARAM_OVERWRITE)->set_allowed_values({PARAM_OVERWRITE, PARAM_APPEND, PARAM_INCREMENTAL_SUFFIX});
}

/**
 * Validate the generic parameters ensuring that
 * things like time step and year are not specified
 * when the report is not running in the execute phase.
 */
void Report::Validate(shared_ptr<Model> model) {
  Report::lock_.lock();
  parameters_.Populate(model);
  DoValidate(model);
  Report::lock_.unlock();
}

/**
 *
 */
void Report::Build(shared_ptr<Model> model) {
  Report::lock_.lock();
  if (time_step_ != "" && !model->managers()->time_step()->GetTimeStep(time_step_))
    LOG_ERROR_P(PARAM_TIME_STEP) << ": " << time_step_ << " could not be found. Have you defined it?";

  DoBuild(model);
  Report::lock_.unlock();
}

/**
 * Check to see if the report has
 * the current year defined as a year
 * when it is suppose to run
 *
 * @param year The year to check
 * @return True if present, false otherwise
 */
bool Report::HasYear(unsigned year) {
  return (std::find(years_.begin(), years_.end(), year) != years_.end());
}

/**
 * The prepare method is called once before the model is run. It is done
 * post-build in the model and allows the report to check if
 * the file it wants to write to exists, etc.
 */
void Report::Prepare(shared_ptr<Model> model) {
  LOG_FINEST() << "preparing report: " << label_;
  Report::lock_.lock();
  SetUpInternalStates();
  DoPrepare(model);
  Report::lock_.unlock();
};

/**
 * Execute the report
 */
void Report::Execute(shared_ptr<Model> model) {
  Report::lock_.lock();
  if (model == nullptr)
    LOG_CODE_ERROR() << "(model == nullptr)";

  DoExecute(model);
  Report::lock_.unlock();
}

/**
 * Finalise the report
 */
void Report::Finalise(shared_ptr<Model> model) {
  Report::lock_.lock();
  DoFinalise(model);
  Report::lock_.unlock();
};

/**
 * Prepare the report
 */
void Report::PrepareTabular(shared_ptr<Model> model) {
  LOG_FINEST() << "preparing tabular report: " << label_;
  // Put a header in

  Report::lock_.lock();
  SetUpInternalStates();

  // Put a header in each file. this is for R library compatibility more than anything.
  if (file_name_ != "" && write_mode_ == PARAM_OVERWRITE)
    cache_ << model->global_configuration().standard_header() << "\n";
  DoPrepareTabular(model);
  Report::lock_.unlock();
}

/**
 * Execute the tabular report
 */
void Report::ExecuteTabular(shared_ptr<Model> model) {
  Report::lock_.lock();
  DoExecuteTabular(model);
  Report::lock_.unlock();
}

/**
 * Finalise the tabular report
 */
void Report::FinaliseTabular(shared_ptr<Model> model) {
  Report::lock_.lock();
  DoFinaliseTabular(model);
  Report::lock_.unlock();
}

/**
 * Set up the internal states for the report
 */
void Report::SetUpInternalStates() {
  /**
   * Figure out the write options. If we're using an incremental suffix
   * we want to loop over the existing files to see what suffix to use.
   */
  if (file_name_ != "" && write_mode_ == PARAM_INCREMENTAL_SUFFIX) {
    LOG_FINEST() << "Finding incremental suffix for file: " << label_;
    if (DoesFileExist(file_name_)) {
      for (unsigned i = 0; i < 10000000; ++i) {
        string trial_name = file_name_ + "." + util::ToInline<unsigned, string>(i);
        if (!DoesFileExist(trial_name)) {
          LOG_FINE() << "File name has been changed to " << trial_name << " to match incremental_suffix";
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
 *
 */
void Report::set_suffix(string_view suffix) {
  Report::lock_.lock();
  suffix_ = suffix;
  Report::lock_.unlock();
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
    bool overwrite = false;
    if (first_write_ || suffix_ != last_suffix_)
      overwrite = overwrite_;

    last_suffix_     = suffix_;
    string file_name = file_name_ + suffix_;

    ios_base::openmode mode = ios_base::out;
    if (!overwrite)
      mode = ios_base::app;

    // Try to Open our File
    ofstream file;
    file.open(file_name.c_str(), mode);
    if (!file.is_open())
      LOG_ERROR() << "Unable to open file: " << file_name;

    LOG_MEDIUM() << "skip tags = " << skip_tags_;
    if (!skip_tags_) {
      cache_ << CONFIG_END_REPORT << "\n";
      // cerr << "end report\n";
    }
    file << cache_.str();
    file.close();

    first_write_ = false;

  } else {
    cout << cache_.str();
    if (!skip_tags_) {
      cout << CONFIG_END_REPORT << "\n";
    }

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
