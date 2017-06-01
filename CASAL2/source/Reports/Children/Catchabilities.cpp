/*
 * Catchabilities.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "Catchabilities.h"

#include "Catchabilities/Manager.h"

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
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";

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

}

/**
 *
 */
void Catchabilities::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
