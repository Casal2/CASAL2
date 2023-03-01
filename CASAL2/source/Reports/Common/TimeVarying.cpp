/**
 * @file TimeVarying.cpp
 * @author  C. Marsh
 * @date 01/06/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

#include "TimeVarying.h"

#include "../../TimeVarying/Manager.h"

namespace niwa {
namespace reports {

/**
 * Default constructor
 */
TimeVarying::TimeVarying() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

  parameters_.Bind<string>(PARAM_TIME_VARYING, &time_varying_label_, "The time varying label that is reported", "", "");
}

/**
 * Validate object
 */
void TimeVarying::DoValidate(shared_ptr<Model> model) {}

/**
 *  Build object
 */
void TimeVarying::DoBuild(shared_ptr<Model> model) {
  // If user supplied
  if (time_varying_label_ == "")
    time_varying_label_ = label_;  // if no explicit time-varying-label given try the report label
  timevarying_ = model->managers()->time_varying()->GetTimeVarying(time_varying_label_);
  if (!timevarying_) {
#ifndef TESTMODE
    LOG_WARNING_Q(PARAM_TIME_VARYING) << "the report for " << PARAM_TIME_VARYING << " with label '" << time_varying_label_ << "' was requested. This " << PARAM_TIME_VARYING
                                      << " was not found in the input configuration file and the report will not be generated";
#endif
    is_valid_ = false;
  }
}

/**
 * Execute the report
 */
void TimeVarying::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  // single valid time-varying report value
  if (is_valid_) {
    cache_ << ReportHeader(type_, time_varying_label_, format_);
    LOG_FINEST() << "Reporting for @time_varying block " << time_varying_label_;
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;

    map<unsigned, Double>& parameter_by_year = timevarying_->get_parameter_by_year();
    cache_ << "year"
           << " Value" << REPORT_EOL;
    for (auto param : parameter_by_year) {
      cache_ << param.first << "  " << AS_DOUBLE(param.second) << REPORT_EOL;
    }
    ready_for_writing_ = true;
  }
}
/**
 * Prepare Tabular report write header output
 */
void TimeVarying::DoPrepareTabular(shared_ptr<Model> model) {}

/**
 * Execute Tabular report
 */
void TimeVarying::DoExecuteTabular(shared_ptr<Model> model) {}

/**
 *
 */
void TimeVarying::DoFinaliseTabular(shared_ptr<Model> model) {}

} /* namespace reports */
} /* namespace niwa */
