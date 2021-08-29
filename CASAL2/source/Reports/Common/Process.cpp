/**
 * @file Process.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include <boost/algorithm/string/join.hpp>

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Processes/Manager.h"
#include "../../Processes/Process.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Process::Process() {
  model_state_ = State::kIterationComplete;
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProjection | RunMode::kProfiling);

  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "The process label that is reported", "", "");
}

/**
 * Validate object
 */
void Process::DoValidate(shared_ptr<Model> model) {
  if (process_label_ == "")
    process_label_ = label_;
}

/**
 * Build the relationships between this object and other objects
 */
void Process::DoBuild(shared_ptr<Model> model) {
  process_ = model->managers()->process()->GetProcess(process_label_);
  if (!process_) {
#ifndef TESTMODE
    LOG_WARNING() << "The report for " << PARAM_PROCESS << " with label '" << process_label_ << "' was requested. This " << PARAM_PROCESS
                  << " was not found in the input configuration file and the report will not be generated";
#endif
    is_valid_ = false;
  }
}
/**
 * Execute this report
 */
void Process::DoExecute(shared_ptr<Model> model) {
  process_ = model->managers()->process()->GetProcess(process_label_);
  if (!process_)
    LOG_CODE_ERROR() << "(!process): " << process_label_;

  LOG_FINE() << " printing report " << label_ << " of type " << process_->type();

  bool is_BH_recruitment = (process_->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) | (process_->type() == PARAM_BEVERTON_HOLT);
  cache_ << ReportHeader(type_, process_label_, format_);

  auto parameters = process_->parameters().parameters();
  for (auto parameter : parameters) {
    if (!(is_BH_recruitment && ((parameter.first == PARAM_YCS_YEARS || parameter.first == PARAM_YCS_VALUES)))) {
      // if this process is a Beverton Holt process don't print the parameters ycs_years or ycs_values. The reason is, this is printed in the storeForReport Function within the
      // process The reason this was done was, we can't update the input parameters to include future years in projection mode, specifically we push back on a vector and becomes
      // a nonsensical vector (when doing multiple projections), thus we went down the store for report method.
      cache_ << parameter.first << ": ";
      string line = boost::algorithm::join(parameter.second->current_values(), " ");
      cache_ << line << REPORT_EOL;
    }
  }
  // Fill the rest of the process specific stuff
  process_->FillReportCache(cache_);
  ready_for_writing_ = true;
}

/**
 * Execute this tabular report
 */
void Process::DoExecuteTabular(shared_ptr<Model> model) {
  if (first_run_) {
    first_run_ = false;
    cache_ << ReportHeader(type_, process_label_, format_);
    cache_ << "type: " << process_->type() << REPORT_EOL;
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
    process_->FillTabularReportCache(cache_, true);
  } else
    process_->FillTabularReportCache(cache_, false);
}

/**
 *  Finalise the tabular report
 */
void Process::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}
} /* namespace reports */
} /* namespace niwa */
