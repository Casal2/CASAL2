/*
 * HessianMatrix.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "HessianMatrix.h"

#include "Minimisers/Manager.h"

namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 *
 */
HessianMatrix::HessianMatrix(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kMCMC);
  model_state_ = State::kFinalise;
}

/**
 * If a minimiser pointer exists this report will ask and print for the hessian matrix
 */
void HessianMatrix::DoExecute() {
  /*
   * This reports the Hessian matrix
   */
  LOG_TRACE();
  /*
   * This reports the Covariance, Correlation and Hessian matrix
   */
  auto minimiser_ = model_->managers().minimiser()->active_minimiser();
  if (!minimiser_)
    return;

  hessian_ = minimiser_->hessian_matrix();
  unsigned hessian_size = minimiser_->hessian_size();

  cache_ << "*" << label_ << " " << "(" << type_ << ")" << "\n";
  cache_ << "Hessian_Matrix " << REPORT_R_MATRIX << "\n";
  for (unsigned i = 0; i < hessian_size; ++i) {
    for (unsigned j = 0; j < hessian_size; ++j)
      cache_ << hessian_[i][j] << " ";
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
