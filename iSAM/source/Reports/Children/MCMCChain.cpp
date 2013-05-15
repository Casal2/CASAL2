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
namespace isam {
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
void MCMCChain::Execute() {
  const vector<mcmc::ChainLink>& chain = MCMC::Instance()->chain();

  cout << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE).GetValue<string>() << "\n";

  for (unsigned i = 0; i < chain.size(); ++i) {
    cout << i << ": ";
    cout << chain[i].score_ << ": ";

    for (double value : chain[i].values_)
      cout << value << " ";

    cout << "\n";
  }

  cout << CONFIG_END_REPORT << "\n" << endl;
}

} /* namespace reports */
} /* namespace isam */
