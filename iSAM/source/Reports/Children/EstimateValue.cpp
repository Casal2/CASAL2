/**
 * @file EstimateValue.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "EstimateValue.h"

#include "Estimates/Manager.h"

// namespaces
namespace isam {
namespace reports {

/**
 * Default constructor
 */
EstimateValue::EstimateValue() {
  run_mode_     = (RunMode::Type)(RunMode::kEstimation | RunMode::kProfiling);
  model_state_  = State::kFinalise;
}

/**
 * Destructor
 */
EstimateValue::~EstimateValue() noexcept(true) {
}

/**
 * Execute this report.
 */
void EstimateValue::Execute() {
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

  cout << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE).GetValue<string>() << "\n";

  for (EstimatePtr estimate : estimates) {
    cout << estimate->parameter() << " ";
  }
  cout << "\n";

  for (EstimatePtr estimate : estimates) {
    cout << AS_DOUBLE(estimate->value()) << " ";
  }
  cout << "\n";

  cout << CONFIG_END_REPORT << "\n" << endl;
}

} /* namespace reports */
} /* namespace isam */
