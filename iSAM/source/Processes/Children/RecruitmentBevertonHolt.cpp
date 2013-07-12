/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "RecruitmentBevertonHolt.h"

#include <numeric>

#include "DerivedQuantities/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace isam {
namespace processes {

namespace dc = isam::utilities::doublecompare;

/**
 * default constructor
 */
RecruitmentBevertonHolt::RecruitmentBevertonHolt() {
  LOG_TRACE();

  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_R0);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_AGE);
  parameters_.RegisterAllowed(PARAM_STEEPNESS);
  parameters_.RegisterAllowed(PARAM_SSB);
  parameters_.RegisterAllowed(PARAM_B0);
  parameters_.RegisterAllowed(PARAM_SSB_OFFSET);
  parameters_.RegisterAllowed(PARAM_YCS_VALUES);
  parameters_.RegisterAllowed(PARAM_STANDARDISE_YCS_YEARS);

  RegisterAsEstimable(PARAM_R0, &r0_);
  RegisterAsEstimable(PARAM_STEEPNESS, &steepness_);
}

/**
 *
 */
void RecruitmentBevertonHolt::Validate() {
  model_ = Model::Instance();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_CATEGORIES);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_labels_  = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  r0_               = parameters_.Get(PARAM_R0).GetValue<double>();
  age_              = parameters_.Get(PARAM_AGE).GetValue<unsigned>(model_->min_age());
  steepness_        = parameters_.Get(PARAM_STEEPNESS).GetValue<double>(1.0);
  ssb_              = parameters_.Get(PARAM_SSB).GetValue<string>();
  ssb_offset_       = parameters_.Get(PARAM_SSB_OFFSET).GetValue<unsigned>();
  proportions_      = parameters_.Get(PARAM_PROPORTIONS).GetValues<Double>();
  ycs_values_       = parameters_.Get(PARAM_YCS_VALUES).GetValues<Double>();

  if (parameters_.IsDefined(PARAM_STANDARDISE_YCS_YEARS))
    standardise_ycs_  = parameters_.Get(PARAM_STANDARDISE_YCS_YEARS).GetValues<unsigned>();

  // Ensure defined categories were valid
  for(const string& category : category_labels_) {
    if (!Categories::Instance()->IsValid(category))
      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << category << " is not a valid category");
  }

  if (age_ < model_->min_age())
    LOG_ERROR(parameters_.location(PARAM_AGE) << " (" << age_ << ") cannot be less than the model's min_age (" << model_->min_age << ")");
  if (age_ > model_->max_age())
    LOG_ERROR(parameters_.location(PARAM_AGE) << " (" << age_ << ") cannot be greater than the model's max_age (" << model_->max_age() << ")");

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << " defined need 1 proportion defined per category. There are " << category_labels_.size()
        << " categories and " << proportions_.size() << " proportions defined.");

  RegisterAsEstimable(PARAM_PROPORTIONS, proportions_);
  Double running_total = std::accumulate(proportions_.begin(), proportions_.end(), 0);
  if (!dc::IsOne(running_total))
    LOG_ERROR(parameters_.location(PARAM_PROPORTIONS) << " sum total is " << running_total << " when it should be 1.0");

  // validate the standardise YCS year range
  if (standardise_ycs_.size() == 0) {
    for (unsigned year = model_->start_year(); year <= model_->final_year(); ++year)
      standardise_ycs_.push_back(year - ssb_offset_);

  } else if (standardise_ycs_.size() > 1) {
    for (unsigned i = 1; i < standardise_ycs_.size(); ++i) {
      if (standardise_ycs_[i - 1] >= standardise_ycs_[i])
        LOG_ERROR(parameters_.location(PARAM_STANDARDISE_YCS_YEARS) << " values must be in numeric ascending order. Value "
            << standardise_ycs_[i - 1] << " is not less than " << standardise_ycs_[i]);

    }
  }

  if (standardise_ycs_[0] < model_->start_year() - ssb_offset_)
    LOG_ERROR(parameters_.location(PARAM_STANDARDISE_YCS_YEARS) << " first value is less than the model's start_year");
  if ((*standardise_ycs_.rbegin()) > model_->final_year() - ssb_offset_)
    LOG_ERROR(parameters_.location(PARAM_STANDARDISE_YCS_YEARS) << " final value is greater than the model's final year");

  // check the number of ycs_values_ supplied matches number of years
  unsigned number_of_years = model_->start_year() - model_->final_year() + 1;
  if (ycs_values_.size() != number_of_years)
    LOG_ERROR(parameters_.location(PARAM_YCS_VALUES) << " need to be defined for every year. Expected " << number_of_years << " but got " << ycs_values_.size());

  RegisterAsEstimable(PARAM_YCS_VALUES, ycs_values_);
  for (double value : ycs_values_) {
    if (value < 0.0)
      LOG_ERROR(parameters_.location(PARAM_YCS_VALUES) << " value " << value << " cannot be less than 0.0");
  }


}

/**
 *
 */
void RecruitmentBevertonHolt::Build() {
  derived_quantity_ = derivedquantities::Manager::Instance().GetDerivedQuantity(ssb_);
}

/**
 *
 */
void RecruitmentBevertonHolt::Execute() {

}

} /* namespace processes */
} /* namespace isam */
