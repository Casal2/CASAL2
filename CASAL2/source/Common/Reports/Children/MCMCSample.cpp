/**
 * @file MCMCSample.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCSample.h"

#include "Common/Estimates/Manager.h"
#include "Common/MCMCs/Manager.h"
#include "Common/MCMCs/MCMC.h"
#include "Common/Model/Managers.h"
#include "Common/Utilities/String.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
MCMCSample::MCMCSample(Model* model) : Report(model) {
  run_mode_ = RunMode::kMCMC;
  model_state_ = State::kIterationComplete;
  skip_tags_ = true;
}

/**
 *
 */
void MCMCSample::DoBuild() {
  mcmc_ = model_->managers().mcmc()->active_mcmc();
  if (!mcmc_)
    LOG_CODE_ERROR() << "mcmc_ = model_->managers().mcmc()->active_mcmc();";
}

/**
 *
 */
void MCMCSample::DoPrepare() {
  if (!model_->global_configuration().resume()) {
    cache_ << "*mcmc (mcmc_sample)\n";
    auto estimates = model_->managers().estimate()->GetIsEstimated();
    for (unsigned i = 0; i < estimates.size() - 1; ++i)
      cache_ << estimates[i]->parameter() << " ";
    cache_ << estimates[estimates.size() - 1]->parameter() << "\n";
  }
}

/**
 *    Print out Chain after each iteration
 */
void MCMCSample::DoExecute() {
  if (!mcmc_)
    LOG_CODE_ERROR() << "if (!mcmc_)";

  auto chain = mcmc_->chain();
  unsigned element = chain.size() - 1;
    cache_ << utilities::String::join<Double>(chain[element].values_, " ") << "\n";

  ready_for_writing_ = true;
}

/**
 *
 */
void MCMCSample::DoFinalise() {
  //cache_ << CONFIG_END_REPORT << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
