/*
 * AgeingErrorMatrix.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "CovarianceMatrix.h"


namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 *
 */
CovarianceMatrix::CovarianceMatrix() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation);
  model_state_ = State::kFinalise;
}

/**
 *
 */

void CovarianceMatrix::DoBuild() {

}

void CovarianceMatrix::DoExecute() {
  LOG_TRACE();
  minimiser_ = minimisers::Manager::Instance().active_minimiser();
  covariance_matrix_ = minimiser_->covariance_matrix();

  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "MCMC_covariance" << REPORT_R_MATRIX <<"\n";
  cache_ << " size of covariance matrix ncols = " << covariance_matrix_.size2() << " nrows = " << covariance_matrix_.size1() << "\n";

  for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
     for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
       cache_ << covariance_matrix_ (i,j) << " ";
       if ( j == (covariance_matrix_.size1() - 1))
         cache_ << "\n";
     }
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
