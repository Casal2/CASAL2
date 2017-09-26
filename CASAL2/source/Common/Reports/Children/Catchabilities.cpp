/*
 * Catchabilities.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "Catchabilities.h"

#include "Common/Catchabilities/Manager.h"

namespace niwa {
namespace reports {


/**
 *
 */
Catchabilities::Catchabilities(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProjection | RunMode::kSimulation);
  model_state_ = (State::Type)(State::kFinalise);
}


/**
 *
 */
void Catchabilities::DoExecute() {
  LOG_TRACE();
  catchabilities::Manager& manager = *model_->managers().catchability();
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";

  auto catchabilities = manager.objects();
  for (auto Q : catchabilities) {
    string label =  Q->label();
    cache_ << "label: " << label << " " << REPORT_R_LIST << " \n";
    cache_ << "value: " <<  AS_DOUBLE(Q->q()) << " \n";
  }
  ready_for_writing_ = true;
}


/**
 * Execute Tabular report
 */

void Catchabilities::DoExecuteTabular() {
  LOG_TRACE();
  catchabilities::Manager& manager = *model_->managers().catchability();
  auto catchabilities = manager.objects();
  if (first_run_) {
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    first_run_ = false;

    for (auto& Q : catchabilities) {
      string label =  Q->label();
      cache_ << "catchability[" << label << "] ";
    }
  }
  for (auto& Q : catchabilities) {
    cache_ << AS_DOUBLE(Q->q()) << " ";
  }
  cache_ << "\n";
}

/**
 *
 */
void Catchabilities::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
