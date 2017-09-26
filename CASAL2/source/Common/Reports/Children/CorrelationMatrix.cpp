/*
 * CorrelationMatrix.cpp
 *
 *  Created on: 28/06/2016
 *      Author: C Marsh
 */

#include "CorrelationMatrix.h"

#include "Common/Minimisers/Manager.h"

namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 *
 */
CorrelationMatrix::CorrelationMatrix(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kMCMC);
  model_state_ = State::kIterationComplete;
}

/**
 *
 */
void CorrelationMatrix::DoExecute() {
  LOG_TRACE();
  /*
   * This reports the Covariance, Correlation and Hessian matrix
   */
  auto minimiser_ = model_->managers().minimiser()->active_minimiser();
  if (!minimiser_)
    return;

  correlation_matrix_ = minimiser_->correlation_matrix();

  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "correlation_matrix " << REPORT_R_MATRIX << "\n";
  for (unsigned i = 0; i < correlation_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < correlation_matrix_.size2(); ++j)
      cache_ << AS_DOUBLE(correlation_matrix_(i, j)) << " ";
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
