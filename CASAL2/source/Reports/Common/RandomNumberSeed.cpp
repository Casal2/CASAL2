/**
 * @file RandomNumberSeed.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 13/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "RandomNumberSeed.h"

#include "../../GlobalConfiguration/GlobalConfiguration.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
RandomNumberSeed::RandomNumberSeed() {
  model_state_ = State::kFinalise;
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation);
}

/**
 * Execute the report
 */
void RandomNumberSeed::DoExecute(shared_ptr<Model> model) {
  // Header
  //cache_ << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << PARAM_RANDOM_NUMBER_SEED << ": " << model->global_configuration().random_seed() << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
