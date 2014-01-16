/**
 * @file EstimateSummary.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "EstimateSummary.h"

#include "Estimates/Manager.h"

// Namespaces
namespace isam {
namespace reports {

/**
 * Default Constructor
 */
EstimateSummary::EstimateSummary() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation);
  model_state_ = State::kFinalise;
}

/**
 * Destructor
 */
EstimateSummary::~EstimateSummary() noexcept(true) {
}

/**
 * This method will execute our estimate summary report
 */
void EstimateSummary::Execute() {
  // Header
  cout << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
//  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE)->GetValue<string>() << "\n";


  // Print the estimates
  isam::estimates::Manager& estimate_manager = isam::estimates::Manager::Instance();
  vector<EstimatePtr> estimates = estimate_manager.GetObjects();

  for (EstimatePtr estimate : estimates) {
    cout << "\n";
    cout << "Estimate: " << estimate->label() << "\n";
    cout << "Lower Bound: " << estimate->lower_bound() << "\n";
    cout << "Upper Bound: " << estimate->upper_bound() << "\n";
    cout << "parameters:\n";
    map<string, ParameterPtr> parameters = estimate->parameters().parameters();
    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
      cout << iter->first << ": ";
      for (string parameter_value : iter->second->values())
        cout << parameter_value << " ";
      cout << "\n";
    }
  }

  cout << CONFIG_END_REPORT << "\n" << endl;
}

} /* namespace reports */
} /* namespace isam */
