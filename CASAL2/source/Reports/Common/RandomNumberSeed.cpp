/**
 * @file RandomNumberSeed.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 13/04/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
  // cache_ << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << PARAM_RANDOM_NUMBER_SEED << ": " << model->global_configuration().random_seed() << REPORT_EOL;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
