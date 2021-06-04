/*
 * CorrelationMatrix.cpp
 *
 *  Created on: 28/06/2016
 *      Author: C Marsh
 */

#include "CorrelationMatrix.h"

#include "../../Minimisers/Manager.h"

namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;

/**
 * Default constructor
 */
CorrelationMatrix::CorrelationMatrix() {
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kMCMC);
  model_state_ = State::kFinalise;
}

/**
 * Execute the report
 */
void CorrelationMatrix::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  /*
   * This reports the covariance, correlation and Hessian matrix
   */
  auto minimiser_ = model->managers()->minimiser()->active_minimiser();
  if (!minimiser_)
    return;

  correlation_matrix_ = minimiser_->correlation_matrix();

  cache_ << "*" << type_ << "[" << label_ << "]"
         << "\n";
  cache_ << "correlation_matrix " << REPORT_R_MATRIX << "\n";
  for (unsigned i = 0; i < correlation_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < correlation_matrix_.size2(); ++j) cache_ << correlation_matrix_(i, j) << " ";
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
