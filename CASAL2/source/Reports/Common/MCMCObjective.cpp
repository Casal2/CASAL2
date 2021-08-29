/**
 * @file MCMCObjective.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCObjective.h"

#include "../../Estimates/Manager.h"
#include "../../MCMCs/MCMC.h"
#include "../../MCMCs/Manager.h"
#include "../../Model/Managers.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
MCMCObjective::MCMCObjective() {
  run_mode_    = RunMode::kMCMC;
  model_state_ = State::kIterationComplete;
  skip_tags_   = true;
}

/**
 * Build the MCMCObjective object
 */
void MCMCObjective::DoBuild(shared_ptr<Model> model) {
  mcmc_ = model->managers()->mcmc()->active_mcmc();
  if (!mcmc_)
    LOG_CODE_ERROR() << "mcmc_ = model_->managers()->mcmc()->active_mcmc();";
}

/**
 * Prepare the MCMCObjective object
 */
void MCMCObjective::DoPrepare(shared_ptr<Model> model) {
  if (!model->global_configuration().resume_mcmc()) {
    cache_ << ReportHeader(type_, label_, format_);
  }
}

/**
 * Print out the MCMCObjective values after each iteration
 */
void MCMCObjective::DoExecute(shared_ptr<Model> model) {
  if (!mcmc_)
    LOG_CODE_ERROR() << "!mcmc_";
  if (!model)
    LOG_CODE_ERROR() << "!model";

  if (first_write_ && !model->global_configuration().resume_mcmc()) {
    /// Up here!!!!!!!!!
    vector<Estimate*> estimates = model->managers()->estimate()->GetIsEstimated();
    cache_ << "starting_covariance_matrix " << REPORT_R_MATRIX << REPORT_EOL;
    auto covariance = mcmc_->covariance_matrix();
    if (estimates.size() != covariance.size1())
      LOG_CODE_ERROR() << "different number of estimates to what is in the covariance matrix. estimates.size() != covariance.size1()";
    // change so there is no trailing space
    cache_ << estimates[0]->parameter();

    for (unsigned i = 1; i < estimates.size(); ++i) {
      cache_ << " " << estimates[i]->parameter();
    }

    cache_ << REPORT_EOL;
    for (unsigned i = 0; i < covariance.size1(); ++i) {
      for (unsigned j = 0; j < covariance.size2() - 1; ++j) cache_ << covariance(i, j) << " ";
      cache_ << covariance(i, covariance.size2() - 1) << REPORT_EOL;
    }

    cache_ << "samples " << REPORT_R_DATAFRAME << REPORT_EOL;
    cache_ << "sample state objective_score prior likelihood penalties additional_priors jacobians step_size acceptance_rate acceptance_rate_since_adapt" << REPORT_EOL;
    first_write_ = false;
  }

  auto chain = mcmc_->chain();
  if (chain.size() >= 1) {
    unsigned element = chain.size() - 1;
    cache_ << chain[element].iteration_ << " " << chain[element].mcmc_state_ << " " << chain[element].score_ << " " << chain[element].prior_ << " " << chain[element].likelihood_
           << " " << chain[element].penalty_ << " " << chain[element].additional_priors_ << " " << chain[element].jacobians_ << " " << chain[element].step_size_ << " "
           << chain[element].acceptance_rate_ << " " << chain[element].acceptance_rate_since_adapt_ << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

/**
 * Finalise the MCMCObjective report
 */
void MCMCObjective::DoFinalise(shared_ptr<Model> model) {
  // cache_ << REPORT_END << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
