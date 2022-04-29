/**
 * @file Basic.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Basic.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * Default constructor
 */
Basic::Basic(shared_ptr<Model> model) : LengthWeight(model) {
  parameters_.Bind<Double>(PARAM_A, &a_, "The $a$ parameter ($W = a L^b$)", "")->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_B, &b_, "The $b$ parameter ($W = a L^b$)", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_UNITS, &units_, "The units for weights (grams, kilograms (kgs), or tonnes)", "")->set_allowed_values({PARAM_TONNES, PARAM_KGS, PARAM_KILOGRAMS, PARAM_GRAMS});
  //parameters_.Bind<Double>(PARAM_LENGTH_BIN_PROPORTION, &length_bin_proportion_, "The proportion between length bins when calculating mean weight calculations", "", 0.5)->set_partition_type(PartitionType::kLength);

  RegisterAsAddressable(PARAM_B, &b_);
  RegisterAsAddressable(PARAM_A, &a_);
}

/**
 * Validate the values passed in from the configuration file.
 *
 * Check that neither 'a' or 'b' are less than or equal to 0.0
 */
void Basic::DoValidate() {
  if (a_ <= 0.0)
    LOG_ERROR_P(PARAM_A) << " (" << AS_DOUBLE(a_) << ") cannot be less than or equal to 0.0";
  if (b_ <= 0.0)
    LOG_ERROR_P(PARAM_B) << " (" << AS_DOUBLE(b_) << ") cannot be less than or equal to 0.0";
}

/**
 * Build
 */
void Basic::DoBuild() {
  if ((units_ == PARAM_TONNES) && (model_->base_weight_units() == PARAM_KGS || model_->base_weight_units() == PARAM_KILOGRAMS))
    unit_multipier_ = 1000;
  else if (units_ == PARAM_GRAMS && (model_->base_weight_units() == PARAM_KGS || model_->base_weight_units() == PARAM_KILOGRAMS))
    unit_multipier_ = 0.001;

  // Deal with base weight as tonnes
  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (model_->base_weight_units() == PARAM_TONNES))
    unit_multipier_ = 0.001;
  else if (units_ == PARAM_GRAMS && (model_->base_weight_units() == PARAM_TONNES))
    unit_multipier_ = 0.000001;

  // Deal with base weight as grams
  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (model_->base_weight_units() == PARAM_GRAMS))
    unit_multipier_ = 1000;
  else if (units_ == PARAM_TONNES && (model_->base_weight_units() == PARAM_GRAMS))
    unit_multipier_ = 1000000;
}

/**
 * Calculate and return the mean weight for the size passed in
 *
 * @param size The size to calculate mean weight for
 * @return The mean weight at the parameter size
 */
Double Basic::mean_weight(Double size, Distribution distribution, Double cv) const {
  Double weight = a_ * pow(size, b_);

  // Add distribution offset
  if (distribution == Distribution::kNormal || distribution == Distribution::kLogNormal)
    weight = weight * pow(1.0 + cv * cv, b_ * (b_ - 1.0) / 2.0);  // Give an R example/proof of this theory

  weight *= unit_multipier_;
  return weight;
}

/**
 * Calculate and return the mean weight for the size passed in
 *
 * @param size The size to calculate mean weight for
 * @return The mean weight at the parameter size
 */
Double Basic::mean_weight(Double size) const {
  return a_ * pow(size, b_);
}
} /* namespace lengthweights */
} /* namespace niwa */
