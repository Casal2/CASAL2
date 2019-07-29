/**
 * @file OutputParameters.cpp
 * @author  C. Marsh
 * @date 15/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "OutputParameters.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Profiles/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
OutputParameters::OutputParameters(Model* model) : Report(model) {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
  model_state_  = State::kIterationComplete;
}

/**
 * Destructor
 */
OutputParameters::~OutputParameters() noexcept(true) {
}

/**
 * Execute this report.
 */
void OutputParameters::DoExecute() {
  vector<Estimate*> estimates = model_->managers().estimate()->objects();
  vector<Profile*>  profiles  = model_->managers().profile()->objects();

  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
     first_run_ = false;
     if (!skip_tags_) {
       cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
       cache_ << "values "<< REPORT_R_MATRIX << "\n";
     }
     for (Estimate* estimate : estimates)
         cache_ << estimate->parameter() << " ";

     if (model_->run_mode() == RunMode::kProfiling) {
       for (auto profile : profiles)
         cache_ << profile->parameter() << " ";
     }
     cache_ << "\n";

   }


   for (Estimate* estimate : estimates)
     cache_ << AS_DOUBLE(estimate->value()) << " ";
   if (model_->run_mode() == RunMode::kProfiling) {
     for (Profile* profile : profiles)
       cache_ << AS_DOUBLE(profile->value()) << " ";
   }
   cache_ << "\n";

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
