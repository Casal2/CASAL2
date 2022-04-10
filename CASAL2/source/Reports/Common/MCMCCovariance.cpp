/**
 * @file MCMCCovariance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCCovariance.h"

#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace ublas = boost::numeric::ublas;
/**
 * Default Constructor
 */
MCMCCovariance::MCMCCovariance() {
  run_mode_    = RunMode::kMCMC;
  model_state_ = State::kFinalise;
}
/**
 *
 *
 */
void MCMCCovariance::DoExecute(shared_ptr<Model> model) {
  MCMC*                  mcmc              = model->managers()->mcmc()->active_mcmc();
  ublas::matrix<double>& covariance_matrix = mcmc->covariance_matrix();

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "MCMC_covariance" << REPORT_R_MATRIX << REPORT_EOL;
  for (unsigned i = 0; i < covariance_matrix.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix.size2(); ++j) {
      cache_ << covariance_matrix(i, j) << " ";
      if (j == (covariance_matrix.size1() - 1))
        cache_ << REPORT_EOL;
    }
  }

  ready_for_writing_ = true;
}

void MCMCCovariance::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_MCMC_COVARIANCE << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
