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
EstimateValue::EstimateValue() {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
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
void EstimateValue::DoExecute() {
  ModelPtr model = Model::Instance();

  vector<EstimatePtr> estimates = estimates::Manager::Instance().objects();
  vector<ProfilePtr>  profiles  = profiles::Manager::Instance().objects();

  /**
   * if this is the first run we print the report header etc
   */
  if (first_run_) {
     first_run_ = false;
     if (!skip_tags_) {
       cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
       cache_ << "values "<< REPORT_R_MATRIX << "\n";
     }
     for (EstimatePtr estimate : estimates)
         cache_ << estimate->parameter() << " ";

     if (model->run_mode() == RunMode::kProfiling) {
       for (ProfilePtr profile : profiles)
         cache_ << profile->parameter() << " ";
     }
     cache_ << "\n";

   }


   for (EstimatePtr estimate : estimates)
     cache_ << AS_DOUBLE(estimate->value()) << " ";
   if (model->run_mode() == RunMode::kProfiling) {
     for (ProfilePtr profile : profiles)
       cache_ << profile->value() << " ";
   }
   cache_ << "\n";

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
