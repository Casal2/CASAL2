/**
 * @file MCMCCovariance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCCovariance.h"

#include "MCMCs/MCMC.h"
#include "MCMCs/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 * Default Constructor
 */
MCMCCovariance::MCMCCovariance(Model* model) : Report(model) {
  run_mode_ = RunMode::kMCMC;
  model_state_ = State::kFinalise;
}
/**
 *
 *
 */
void MCMCCovariance::DoExecute() {
  MCMC* mcmc = model_->managers().mcmc()->active_mcmc();
  ublas::matrix<Double>& covariance_matrix = mcmc->covariance_matrix();

  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "MCMC_covariance" << REPORT_R_MATRIX <<"\n";
  for (unsigned i = 0; i < covariance_matrix.size1(); ++i) {
     for (unsigned j = 0; j < covariance_matrix.size2(); ++j) {
       cache_ << AS_DOUBLE(covariance_matrix(i,j)) << " ";
       if ( j == (covariance_matrix.size1() - 1))
         cache_ << "\n";
     }
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
