/*
 * AgeingErrorMatrix.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "CovarianceMatrix.h"

#include "Minimisers/Manager.h"
#include "MCMCs/Manager.h"

namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 *
 */
CovarianceMatrix::CovarianceMatrix(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kMCMC);
  model_state_ = State::kFinalise;
}

/**
 *
 */

void CovarianceMatrix::DoBuild() {
  mcmc_ = model_->managers().mcmc()->active_mcmc();
  if (!mcmc_)
    LOG_CODE_ERROR() << "mcmc_ = model_->managers().mcmc()->active_mcmc();";

}

void CovarianceMatrix::DoExecute() {
  /*
   * This reports the Covariance, Correlation and Hessian matrix
   */
  LOG_TRACE();
  minimiser_ = model_->managers().minimiser()->active_minimiser();
  covariance_matrix_ = minimiser_->covariance_matrix();
  correlation_matrix_ = minimiser_->correlation_matrix();
  hessian_ = minimiser_->hessian_matrix();
  unsigned hessian_size  = minimiser_->hessian_size();

  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "Minimiser_Covariance_Matrix" << REPORT_R_MATRIX <<"\n";

  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
     for (unsigned j = 0; j < covariance_matrix_.size2(); ++j)
       cache_ << covariance_matrix_ (i,j) << " ";
     cache_ << "\n";
  }

  cache_ << "*" << "Correlation_Matrix" << REPORT_R_MATRIX <<"\n";
  for (unsigned i = 0; i < correlation_matrix_.size1(); ++i) {
    for (unsigned j = 0; j < correlation_matrix_.size2(); ++j)
      cache_ << correlation_matrix_ (i,j) << " ";
    cache_ << "\n";
  }

  cache_ << "*" << "Hessian_Matrix" << REPORT_R_MATRIX <<"\n";
  for (unsigned i = 0; i < hessian_size; ++i) {
     for (unsigned j = 0; j < hessian_size; ++j)
       cache_ << hessian_ [i][j] << " ";
     cache_ << "\n";
  }
  if (mcmc_->recalculate_covariance()) {
    LOG_FINE() << "During the mcmc run you recalculated the the covariance matrix, so we will print the modified matrix at the end of the chain";
    cache_ << "*" << "Modified_covariance_matrix" << " " << REPORT_R_MATRIX << "\n";
    auto covariance = mcmc_->covariance_matrix();
    for (unsigned i = 0; i < covariance.size1(); ++i) {
       for (unsigned j = 0; j < covariance.size2() - 1; ++j)
         cache_ << covariance(i,j) << " ";
       cache_ << covariance(i, covariance.size2() - 1) << "\n";
    }
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
