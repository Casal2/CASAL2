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
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProjection | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kFinalise);
}

/**
 * Execute the report
 */
void Catchability::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  catchabilities::Manager& manager = *model->managers()->catchability();
  cache_ << "*" << type_ << "[" << label_ << "]"
         << "\n";

  auto catchabilities = manager.objects();
  for (auto Q : catchabilities) {
    string label = Q->label();
    cache_ << label << ": " << AS_DOUBLE(Q->q()) << " \n";
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
    cache_ << "*" << type_ << "[" << label_ << "]"
           << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    first_run_ = false;

    for (auto& Q : catchabilities) {
      string label = Q->label();
      cache_ << "catchability[" << label << "] ";
    }
    cache_ << "\n";
  }
  for (auto& Q : catchabilities) {
    cache_ << AS_DOUBLE(Q->q()) << " ";
  }
  cache_ << "\n";
}

/**
 * Finalise the tabular report
 */
void Catchability::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
