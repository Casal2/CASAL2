/**
 * @file MigrationSS3.cpp
 * @author  S Datta
 * @version 1.0
 * @date 2024
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2024-10-02 12:19:00 +1300 (Wed, 02 Oct 2024) $
 */

// Headers
#include "MigrationSS3.h"

#include "../../Model/Model.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
MigrationSS3::MigrationSS3(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<Double>(PARAM_L, &min_set_age_, "Lower age to specifiy migration rate for.", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_H, &max_set_age_, "Upper age to specifiy migration rate for.", "");
  parameters_.Bind<Double>(PARAM_Y1, &migration_rate_at_L_, "Migration rate at lower age, between 0 and 1", "")->set_range(0.0, 1.0, true, true);
  parameters_.Bind<Double>(PARAM_Y2, &migration_rate_at_H_, "Migration rate at upper age, between 0 and 1", "")->set_range(0.0, 1.0, true, true);
  parameters_.Bind<Double>(PARAM_ALT_RATE_LOWER_SWITCH, &alt_rate_lower_switch_, "Switch for setting different value below lower age.", "")->set_range(0.0, 1.0, true, true);
  parameters_.Bind<Double>(PARAM_ALT_RATE_UPPER_SWITCH, &alt_rate_upper_switch_, "Switch for setting different value above upper age.", "")->set_range(0.0, 1.0, true, true);
  parameters_.Bind<Double>(PARAM_ALT_RATE_LOWER, &alt_rate_lower_, "Value of migration below lower age.", "")->set_range(0.0, 1.0, true, true);
  parameters_.Bind<Double>(PARAM_ALT_RATE_UPPER, &alt_rate_upper_, "Value of migration below lower age.", "")->set_range(0.0, 1.0, true, true);
  
  
  RegisterAsAddressable(PARAM_L, &min_set_age_);
  RegisterAsAddressable(PARAM_H, &max_set_age_);
  RegisterAsAddressable(PARAM_Y1, &migration_rate_at_L_);
  RegisterAsAddressable(PARAM_Y2, &migration_rate_at_H_);
  RegisterAsAddressable(PARAM_ALT_RATE_LOWER_SWITCH, &alt_rate_lower_switch_);
  RegisterAsAddressable(PARAM_ALT_RATE_UPPER_SWITCH, &alt_rate_upper_switch_);
  RegisterAsAddressable(PARAM_ALT_RATE_LOWER, &alt_rate_lower_);
  RegisterAsAddressable(PARAM_ALT_RATE_UPPER, &alt_rate_upper_);
  allowed_length_based_in_age_based_model_ = false;

}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * Then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void MigrationSS3::DoValidate() {

}

void MigrationSS3::DoReset() {
  r_ = 1e-4; // small amount to add to values
  y1dash_ = migration_rate_at_L_ + r_;
  y2dash_ = migration_rate_at_H_ + r_;
  k_ = log(y2dash_/y1dash_)/(max_set_age_ - min_set_age_);
  if (alt_rate_lower_switch_ > 0) {
	yyoung_ = alt_rate_lower_;
  } else {
	yyoung_ = migration_rate_at_L_;  
  }
  if (alt_rate_upper_switch_ > 0) {
	yold_ = alt_rate_upper_;
  } else {
	yold_ = migration_rate_at_H_;  
  }
}
/**
 * The core function
 */
Double MigrationSS3::get_value(Double value) {
  Double sel_val = 0.0;
  // NOTE: NEED to get the model min and max age or min and max size, and the bin width (assumed to be 1 below)
  // min age is popn.state.col_min or get_int("min_age")
  // max age is popn.state.col_max or get_int("max_age")
  // length bins is popn.state.class_mins; min size is the value in the first bin and max size is the value in the last bin

  // from Ian Doonan, sent on 2024-10-02
  if (value < min_set_age_) {
	sel_val = yyoung_;
  } else if (value > max_set_age_) {
	sel_val = yold_;
  } else {
	sel_val = y1dash_ * exp(k_*(value - min_set_age_));
  }
  return sel_val;
}

/**
 * The core function
 */
Double MigrationSS3::get_value(unsigned value) {
 Double sel_val = 0.0;
  // NOTE: NEED to get the model min and max age or min and max size, and the bin width (assumed to be 1 below)
  // min age is popn.state.col_min or get_int("min_age")
  // max age is popn.state.col_max or get_int("max_age")
  // length bins is popn.state.class_mins; min size is the value in the first bin and max size is the value in the last bin

  // from Ian Doonan, sent on 2024-10-02
  if (value < min_set_age_) {
	sel_val = yyoung_;
  } else if (value > max_set_age_) {
	sel_val = yold_;
  } else {
	sel_val = y1dash_ * exp(k_*(value - min_set_age_));
  }
  return sel_val;
}

} /* namespace selectivities */
} /* namespace niwa */
