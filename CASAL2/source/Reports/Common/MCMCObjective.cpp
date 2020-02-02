/**
 * @file MCMCObjective.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCObjective.h"

#include "Estimates/Manager.h"
#include "MCMCs/Manager.h"
#include "MCMCs/MCMC.h"
#include "Model/Managers.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
MCMCObjective::MCMCObjective(Model* model) : Report(model) {
  run_mode_     = RunMode::kMCMC;
  model_state_  = State::kIterationComplete;
  skip_tags_    = true;
}

/**
 *
 */
void MCMCObjective::DoBuild() {
  mcmc_ = model_->managers().mcmc()->active_mcmc();
  if (!mcmc_)
    LOG_CODE_ERROR() << "mcmc_ = model_->managers().mcmc()->active_mcmc();";
}

/**
 *
 */
void MCMCObjective::DoPrepare() {
  if (!model_->global_configuration().resume()) {
    cache_ << "*mcmc_objective[mcmc]" << "\n";
  }
}

/**
 *    Print out Chain after each iteration
 */
void MCMCObjective::DoExecute() {
  if (!mcmc_)
    LOG_CODE_ERROR() << "if (!mcmc_)";

  if (first_write_ && !model_->global_configuration().resume()) {
  	/// Up here!!!!!!!!!
  	vector<Estimate*>  estimates = model_->managers().estimate()->GetIsEstimated();
    cache_ << "starting_covariance_matrix {m}\n";
    auto covariance = mcmc_->covariance_matrix();
    if (estimates.size() != covariance.size1())
      LOG_CODE_ERROR() << "different number of estimates to what are in the covariance matrix. estimates.size() != covariance.size1()";
    // change so there is no trailing space
    cache_ << estimates[0]->parameter();
    for (unsigned i = 1; i < estimates.size(); ++i) {
      cache_ << " " << estimates[i]->parameter();
    }
    cache_ << "\n";
    for (unsigned i = 0; i < covariance.size1(); ++i) {
       for (unsigned j = 0; j < covariance.size2() - 1; ++j)
         cache_ << covariance(i,j) << " ";
       cache_ << covariance(i, covariance.size2() - 1) << "\n";
    }
    cache_ << "samples {d} \n";
    cache_ << "sample objective_score prior likelihood penalties additional_priors jacobians step_size acceptance_rate acceptance_rate_since_adapt\n";
  }

  auto chain = mcmc_->chain();
  unsigned element = chain.size() - 1;
    cache_ << chain[element].iteration_ << " "
        << AS_DOUBLE(chain[element].score_) << " "
        << AS_DOUBLE(chain[element].prior_) << " "
        << AS_DOUBLE(chain[element].likelihood_) << " "
        << AS_DOUBLE(chain[element].penalty_) << " "
        << AS_DOUBLE(chain[element].additional_priors_) << " "
        << AS_DOUBLE(chain[element].jacobians_) << " "
        << chain[element].step_size_ << " "
        << chain[element].acceptance_rate_ << " "
        << chain[element].acceptance_rate_since_adapt_ << "\n";

  ready_for_writing_ = true;
}

/**
 *
 */
void MCMCObjective::DoFinalise() {
  //cache_ << CONFIG_END_REPORT << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
