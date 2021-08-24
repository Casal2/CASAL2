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
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kEstimation | RunMode::kSimulation);
  model_state_ = (State::Type)(State::kIterationComplete);
}

/**
 * Execute the report
 */
void TimeVarying::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  auto manager      = model->managers()->time_varying();
  auto time_varying = manager->objects();
  if (manager->size() > 0 && time_varying.size() > 0) {
    cache_ << ReportHeader(type_, label_);

    for (auto time_var : time_varying) {
      string label = time_var->label();
      LOG_FINEST() << "Reporting for @time_varying block " << label;
      cache_ << label << " " << REPORT_R_DATAFRAME << REPORT_EOL;

      map<unsigned, Double>& parameter_by_year = time_var->get_parameter_by_year();
      cache_ << "year"
             << " Value" << REPORT_EOL;
      for (auto param : parameter_by_year) {
        cache_ << param.first << "  " << AS_DOUBLE(param.second) << REPORT_EOL;
      }
    }
    ready_for_writing_ = true;
  }
}

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
