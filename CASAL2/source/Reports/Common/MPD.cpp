/**
 * @file MPD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 30/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "MPD.h"

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Managers.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Constructor
 *
 * @param model A pointer to the model this report is linked to
 */
MPD::MPD() {
  run_mode_    = RunMode::kEstimation;
  model_state_ = State::kFinalise;
}

/**
 * Execute this report
 */
void MPD::DoExecute(shared_ptr<Model> model) {
  cache_ << ReportHeader(type_, label_, format_);
  /**
   * Print our Estimate Values
   */
  cache_ << "estimate_values:\n";
  auto estimates = model->managers()->estimate()->GetIsEstimated();
  for (auto estimate : estimates) cache_ << estimate->parameter() << " ";
  cache_ << REPORT_EOL;

  for (auto estimate : estimates) cache_ << AS_DOUBLE(estimate->value()) << " ";
  cache_ << REPORT_EOL;

  /**
   * Print our covariance matrix
   */
  cache_ << "covariance_matrix:\n";
  auto covariance_matrix = model->managers()->minimiser()->active_minimiser()->covariance_matrix();
  for (unsigned i = 0; i < covariance_matrix.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix.size2(); ++j) cache_ << covariance_matrix(i, j) << " ";
    cache_ << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

void MPD::DoExecuteTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_MPD << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
