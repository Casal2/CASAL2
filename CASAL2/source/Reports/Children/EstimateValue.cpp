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
#include "Model/Model.h"
#include "Profiles/Manager.h"

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
  LOG_TRACE();
  vector<Estimate*> estimates = model_->managers().estimate()->objects();
  vector<Profile*> profiles = model_->managers().profile()->objects();

  if (estimates.size() > 0) {
    /**
     * if this is the first run we print the report header etc
     */
    if (first_run_) {
      LOG_FINEST() << "first run of estimate_value report";
      first_run_ = false;
      if (!skip_tags_) {
        cache_ << "*" << label_ << " " << "(" << type_ << ")" << "\n";
        cache_ << "values " << REPORT_R_DATAFRAME << "\n";
      }
      for (Estimate* estimate : estimates)
        cache_ << estimate->parameter() << " ";
      cache_ << "\n";
      LOG_FINEST() << "Number of estimates reporting on = " << estimates.size();
      for (Estimate* estimate : estimates)
        cache_ << AS_DOUBLE(estimate->value()) << " ";
      cache_ << "\n";

    } else {

      if (model_->run_mode() == RunMode::kProfiling) {
        cache_ << "*" << label_ << " " << "(" << type_ << ")" << "\n";
        cache_ << "values " << REPORT_R_DATAFRAME << "\n";
        for (Estimate* estimate : estimates)
          cache_ << estimate->parameter() << " ";
        cache_ << "\n";
        for (Estimate* estimate : estimates)
          cache_ << AS_DOUBLE(estimate->value()) << " ";
      } else {
        LOG_FINEST() << "Number of estimates reporting on = "
            << estimates.size();
        for (Estimate* estimate : estimates)
          cache_ << AS_DOUBLE(estimate->value()) << " ";
      }
      cache_ << "\n";
    }

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
    cache_ << "*" << label_ << " " << "(" << type_ << ")" << "\n";
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
