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

#include "Common/Estimates/Manager.h"
#include "Common/Model/Model.h"
#include "Common/Profiles/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
EstimateValue::EstimateValue(Model* model) : Report(model) {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling | RunMode::kProjection);
  model_state_  = State::kIterationComplete;
}

/**
 * Destructor
 */
EstimateValue::~EstimateValue() noexcept(true) {
}

/**
 * Execute this report.
 */
void EstimateValue::DoExecute() {
  vector<Estimate*> estimates = model_->managers().estimate()->objects();
  vector<Profile*> profiles = model_->managers().profile()->objects();
  LOG_TRACE();
  // Check if estiamtes are close to bounds. flag a warning.
  for (Estimate* estimate : estimates) {
    if ((estimate->value() - estimate->lower_bound()) < 0.001 || (estimate->upper_bound() - estimate->value()) < 0.001)
      LOG_WARNING() << "estimated parameter '" <<  estimate->parameter() << "' was within 0.001 of its bound";
  }

  if (estimates.size() > 0) {
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (Estimate* estimate : estimates)
      cache_ << estimate->parameter() << " ";
    cache_ << "\n";
    for (Estimate* estimate : estimates)
      cache_ << AS_DOUBLE(estimate->value()) << " ";
    cache_ << "\n";
    ready_for_writing_ = true;
  }
}

/**
 *  Execute the report in tabular format
 */
void EstimateValue::DoExecuteTabular() {
  vector<Estimate*> estimates = model_->managers().estimate()->objects();
  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
    first_run_ = false;
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (Estimate* estimate : estimates)
      cache_ << estimate->parameter() << " ";
    cache_ << "\n";
  }

  for (Estimate* estimate : estimates)
    cache_ << AS_DOUBLE(estimate->value()) << " ";
  cache_ <<"\n" ;
}

/**
 *  End report signature
 */
void EstimateValue::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
