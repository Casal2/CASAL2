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
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
EstimateSummary::EstimateSummary(Model* model) : Report(model) {
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
void EstimateSummary::DoExecute() {


  // Print the estimates
  niwa::estimates::Manager& estimate_manager = *model_->managers().estimate();
  vector<Estimate*> estimates = estimate_manager.objects();

  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  for (Estimate* estimate : estimates) {
	cache_ << estimate->parameter() << " " << REPORT_R_LIST << "\n";
 //   cache_ << "label: " << estimate->label() << "\n";
    cache_ << "lower_bound: " << estimate->lower_bound() << "\n";
    cache_ << "upper_bound: " << estimate->upper_bound() << "\n";
    cache_ << "value: " << AS_DOUBLE(estimate->value()) << "\n";
    map<string, Parameter*> parameters = estimate->parameters().parameters();
    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
      cache_ << iter->first << ": ";
      for (string parameter_value : iter->second->values())
        cache_ << parameter_value << " ";
      cache_ << "\n";
    }
    cache_ << REPORT_R_LIST_END << "\n\n";
  }


  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
