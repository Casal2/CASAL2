/**
 * @file OutputParameters.cpp
 * @author  C. Marsh
 * @date 15/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "OutputParameters.h"

#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../Profiles/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
OutputParameters::OutputParameters() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kIterationComplete;
}

/**
 * Execute this report.
 */
void OutputParameters::DoExecute(shared_ptr<Model> model) {
  vector<Estimate*> estimates = model->managers()->estimate()->objects();
  vector<Profile*>  profiles  = model->managers()->profile()->objects();

  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
    first_run_ = false;
    if (!skip_tags_) {
      cache_ << ReportHeader(type_, label_, format_);
      cache_ << "values " << REPORT_R_MATRIX << REPORT_EOL;
    }
    for (Estimate* estimate : estimates) cache_ << estimate->parameter() << " ";

    if (model->run_mode() == RunMode::kProfiling) {
      for (auto profile : profiles) cache_ << profile->parameter() << " ";
    }
    cache_ << REPORT_EOL;
  }

  for (Estimate* estimate : estimates) cache_ << AS_DOUBLE(estimate->value()) << " ";
  if (model->run_mode() == RunMode::kProfiling) {
    for (Profile* profile : profiles) cache_ << AS_DOUBLE(profile->value()) << " ";
  }
  cache_ << REPORT_EOL;

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
