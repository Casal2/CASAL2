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
    cache_ << "*" << label_ << "\n";
  }
}

/**
 *    Print out Chain after each iteration
 */
void MCMCObjective::DoExecute() {
  if (!mcmc_)
    LOG_CODE_ERROR() << "if (!mcmc_)";

  if (first_write_ && !model_->global_configuration().resume()) {
    cache_ << "covariance_matrix:\n";
    auto covariance = mcmc_->covariance_matrix();
    for (unsigned i = 0; i < covariance.size1(); ++i) {
       for (unsigned j = 0; j < covariance.size2() - 1; ++j)
         cache_ << covariance(i,j) << " ";
       cache_ << covariance(i, covariance.size2() - 1) << "\n";
    }
    cache_ << "samples:\n";
    cache_ << "sample objective_score prior likelihood penalties additional_priors jacobians step_size acceptance_rate acceptance_rate_since_adapt\n";
  }

  auto chain = mcmc_->chain();
  unsigned element = chain.size() - 1;
    cache_ << chain[element].iteration_ << " "
        << chain[element].score_ << " "
        << chain[element].prior_ << " "
        << chain[element].likelihood_ << " "
        << chain[element].penalty_ << " "
        << chain[element].additional_priors_ << " "
        << chain[element].jacobians_ << " "
        << chain[element].step_size_ << " "
        << chain[element].acceptance_rate_ << " "
        << chain[element].acceptance_rate_since_adapt_ << "\n";

  ready_for_writing_ = true;
}

/**
 *
 */
void MCMCObjective::DoFinalise() {
  cache_ << CONFIG_END_REPORT << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
