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

#include "MCMC/MCMC.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
MCMCChain::MCMCChain() {
  run_mode_ = RunMode::kMCMC;
  model_state_ = State::kFinalise;
}

/**
 * Destructor
 */
MCMCChain::~MCMCChain() noexcept(true) {

}

/**
 *
 */
void MCMCChain::DoExecute() {
  const vector<mcmc::ChainLink>& chain = MCMC::Instance()->chain();

  cache_ << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
//  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE).GetValue<string>() << "\n";

  for (unsigned i = 0; i < chain.size(); ++i) {
    cache_ << i << ": ";
    cache_ << chain[i].score_ << ": ";

    for (Double value : chain[i].values_)
      cache_ << value << " ";

    cache_ << "\n";
  }

  cache_ << CONFIG_END_REPORT << "\n" << endl;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
