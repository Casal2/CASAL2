/**
 * @file MCMCChain.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCChain.h"

#include "MCMCs/MCMC.h"
#include "MCMCs/Manager.h"
// namespaces
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
MCMCChain::MCMCChain() {
  run_mode_ = RunMode::kMCMC;
  model_state_ = State::kIterationComplete;
  skip_tags_ = true;
}

/**
 * Destructor
 */
MCMCChain::~MCMCChain() noexcept(true) {

}

/**
 *
 *
 */
void MCMCChain::DoExecute() {
  MCMCPtr mcmc = mcmcs::Manager::Instance().active_mcmc();
  vector<mcmc::ChainLink> chain = mcmc->chain();

  if (chain.size() == 2) {
    cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
    cache_ << "MCMC_values" << REPORT_R_DATAFRAME <<"\n";
    cache_ << "index Objective_score prior likelihood penalties step_size acceptance_rate acceptance_rate_since_adapt ";
    for (string labels : mcmc->GetEstimateLabel())
      cache_ << labels << " ";
    cache_ << "\n";
  }

  cache_ << (chain.size() - 1) << " ";
  cache_ << chain[chain.size() - 1].score_ << " " << chain[chain.size() - 1].prior_ << " " << chain[chain.size() - 1].likelihood_ << " "
      << chain[chain.size() - 1].penalty_ << " " << chain[chain.size() - 1].step_size_ << " " << chain[chain.size() - 1].acceptance_rate_ << " "
      << chain[chain.size() - 1].acceptance_rate_since_adapt_;

  for (Double value : chain[chain.size() - 1].values_)
    cache_ << value << " ";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
