/**
 * @file MPD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 30/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MPD.h"

#include "Common/Model/Managers.h"
#include "Common/Estimates/Manager.h"
#include "Common/Minimisers/Manager.h"
#include "Common/Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Constructor
 *
 * @param model A pointer to the model this report is linked to
 */
MPD::MPD(Model* model) : Report(model) {
  run_mode_ = RunMode::kEstimation;
  model_state_ = State::kFinalise;
}

/**
 * Execute this report
 */
void MPD::DoExecute() {
  cache_ << "* MPD\n";

  /**
   * Print our Estimate Values
   */
  cache_ << "estimate_values:\n";
  auto estimates = model_->managers().estimate()->GetIsEstimated();
  for (auto estimate : estimates)
    cache_ << estimate->parameter() << " ";
  cache_ << "\n";

  for (auto estimate : estimates)
    cache_ << AS_DOUBLE(estimate->value()) << " ";
  cache_ << "\n";

  /**
   * Print our covariance matrix
   */
  cache_ << "covariance_matrix:\n";
  auto covariance_matrix = model_->managers().minimiser()->active_minimiser()->covariance_matrix();
  for (unsigned i = 0; i < covariance_matrix.size1(); ++i) {
     for (unsigned j = 0; j < covariance_matrix.size2(); ++j)
       cache_ << covariance_matrix(i,j) << " ";
     cache_ << "\n";
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
