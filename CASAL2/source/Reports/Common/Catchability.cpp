/*
 * Catchability.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "Catchability.h"

#include "../../Catchabilities/Manager.h"

namespace niwa {
namespace reports {

/**
 *
 */
Catchability::Catchability() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The catchability label", "", "");
}
/**
 * Validate object
 */
void Catchability::DoValidate(shared_ptr<Model> model) {
  if (catchability_label_ == "")
    catchability_label_ = label_;
}
/**
 * Build the relationships between this object and other objects
 */
void Catchability::DoBuild(shared_ptr<Model> model) {
  catchability_ = model->managers()->catchability()->GetCatchability(catchability_label_);
  if (!catchability_) {
#ifndef TESTMODE
    LOG_WARNING() << "The report for " << PARAM_CATCHABILITY << " with label '" << catchability_label_ << "' was requested. This " << PARAM_CATCHABILITY
                  << " was not found in the input configuration file and the report will not be generated";
#endif
    is_valid_ = false;
  }
}
/**
 * Execute the report
 */
void Catchability::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  catchabilities::Manager& manager = *model->managers()->catchability();
  cache_ << ReportHeader(type_, catchability_label_, format_);

  auto catchabilities = manager.objects();
  for (auto Q : catchabilities) {
    string label = Q->label();
    cache_ << label << ": " << AS_DOUBLE(Q->q()) << REPORT_EOL;
  }
  ready_for_writing_ = true;
}

/**
 * Execute the tabular report
 */

void Catchability::DoExecuteTabular(shared_ptr<Model> model) {
  LOG_TRACE();
  catchabilities::Manager& manager        = *model->managers()->catchability();
  auto                     catchabilities = manager.objects();
  if (first_run_) {
    cache_ << ReportHeader(type_, catchability_label_, format_);
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
    first_run_ = false;

    for (auto& Q : catchabilities) {
      string label = Q->label();
      cache_ << "catchability[" << label << "] ";
    }
    cache_ << REPORT_EOL;
  }
  for (auto& Q : catchabilities) {
    cache_ << AS_DOUBLE(Q->q()) << " ";
  }
  cache_ << REPORT_EOL;
}

/**
 * Finalise the tabular report
 */
void Catchability::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
