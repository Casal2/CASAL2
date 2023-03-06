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
void Catchability::DoValidate(shared_ptr<Model> model) {}
/**
 * Build the relationships between this object and other objects
 */
void Catchability::DoBuild(shared_ptr<Model> model) {
  if (!model->global_configuration().print_tabular()) {
    if (catchability_label_ == "")
      catchability_label_ = label_;
  }
  if (catchability_label_ != "") {
    catchability_ = model->managers()->catchability()->GetCatchability(catchability_label_);
    if (!catchability_) {
#ifndef TESTMODE
      LOG_WARNING() << "The " << PARAM_CATCHABILITY << " report with label '" << catchability_label_ << "' was requested. This " << PARAM_CATCHABILITY
                    << " was not found in the input configuration file and the report will not be generated";
#endif
      is_valid_ = false;
    }
  }
}
/**
 * Execute the report
 */
void Catchability::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  cache_ << ReportHeader(type_, label_, format_);
  string label = catchability_->label();
  cache_ << "q: " << AS_DOUBLE(catchability_->q()) << REPORT_EOL;

  ready_for_writing_ = true;
}

void Catchability::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_TRACE();
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  if (catchability_label_ != "") {
    // single catchability provided not recommended
    catchability_ = model->managers()->catchability()->GetCatchability(catchability_label_);
    string label  = catchability_->label();
    cache_ << "catchability[" << label << "]";
    cache_ << REPORT_EOL;
  } else {
    catchabilities::Manager& CatchabilityManager = *model->managers()->catchability();
    for (auto object : CatchabilityManager.objects()) {
      string label = object->label();
      cache_ << "catchability[" << label << "] ";
    }
    cache_ << REPORT_EOL;
  }
}
/**
 * Execute the tabular report
 */

void Catchability::DoExecuteTabular(shared_ptr<Model> model) {
  if (catchability_label_ != "") {
    cache_ << AS_DOUBLE(catchability_->q()) << " ";
    cache_ << REPORT_EOL;
  } else {
    catchabilities::Manager& CatchabilityManager = *model->managers()->catchability();
    for (auto object : CatchabilityManager.objects()) {
      cache_ << AS_DOUBLE(object->q()) << " ";
    }
    cache_ << REPORT_EOL;
  }
}

/**
 * Finalise the tabular report
 */
void Catchability::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
