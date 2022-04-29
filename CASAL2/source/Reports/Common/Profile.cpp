/**
 * @file Profile.cpp
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "Profile.h"

#include <boost/algorithm/string/join.hpp>

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Profiles/Manager.h"
#include "../../Profiles/Profile.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Profile::Profile() {
  model_state_ = State::kFinalise;
  run_mode_    = (RunMode::Type)(RunMode::kProfiling);
}
/**
 * Validate object
 */
void Profile::DoValidate(shared_ptr<Model> model) {}

/**
 * Build the relationships between this object and other objects
 */
void Profile::DoBuild(shared_ptr<Model> model) {
  profile_ = model->managers()->profile()->GetProfile();
  if (!profile_) {
#ifndef TESTMODE
    LOG_WARNING() << "The report for " << PARAM_PROFILE << " was requested. There are no @" << PARAM_PROFILE
                  << " blocks found in the input configuration file. The report will not be generated";
#endif
    is_valid_ = false;
  }
}

/**
 * Execute this report
 */
void Profile::DoExecute(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  LOG_FINE() << " printing report " << label_ << " of type " << profile_->type();
  vector<Double> values = profile_->get_values();
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "profile: " << profile_->label() << REPORT_EOL;
  cache_ << "parameter: " << profile_->parameter() << REPORT_EOL;
  string same_param = profile_->same_parameter();
  if (same_param != "")
    cache_ << "same_parameter: " << same_param << REPORT_EOL;
  cache_ << "values: ";
  for (auto value : values) {
    cache_ << value << " ";
  }
  cache_ << REPORT_EOL;
  ready_for_writing_ = true;
}

void Profile::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_PROFILE << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
