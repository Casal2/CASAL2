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

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "InitialisationPhases/Manager.h"
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
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_R0);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_AGE);
  parameters_.RegisterAllowed(PARAM_STEEPNESS);
  parameters_.RegisterAllowed(PARAM_SSB);
  parameters_.RegisterAllowed(PARAM_B0);
  parameters_.RegisterAllowed(PARAM_SSB_OFFSET);
  parameters_.RegisterAllowed(PARAM_YCS_YEARS);
  parameters_.RegisterAllowed(PARAM_YCS_VALUES);
  parameters_.RegisterAllowed(PARAM_STANDARDISE_YCS_YEARS);

  RegisterAsEstimable(PARAM_R0, &r0_);
  RegisterAsEstimable(PARAM_STEEPNESS, &steepness_);

  phase_b0_         = 0;
}

/**
 *
 */
void RecruitmentBevertonHolt::DoValidate() {
  model_ = Model::Instance();

  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_R0);
  CheckForRequiredParameter(PARAM_SSB);
  CheckForRequiredParameter(PARAM_SSB_OFFSET);
  CheckForRequiredParameter(PARAM_PROPORTIONS);
  CheckForRequiredParameter(PARAM_YCS_VALUES);

  label_            = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_labels_  = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  r0_               = parameters_.Get(PARAM_R0).GetValue<double>();
  age_              = parameters_.Get(PARAM_AGE).GetValue<unsigned>(model_->min_age());
  steepness_        = parameters_.Get(PARAM_STEEPNESS).GetValue<double>(1.0);
  ssb_              = parameters_.Get(PARAM_SSB).GetValue<string>();
  ssb_offset_       = parameters_.Get(PARAM_SSB_OFFSET).GetValue<unsigned>();
  proportions_      = parameters_.Get(PARAM_PROPORTIONS).GetValues<Double>();
  ycs_values_       = parameters_.Get(PARAM_YCS_VALUES).GetValues<Double>();
  phase_b0_label_   = parameters_.Get(PARAM_B0).GetValue<string>("");

  if (parameters_.IsDefined(PARAM_STANDARDISE_YCS_YEARS))
    standardise_ycs_  = parameters_.Get(PARAM_STANDARDISE_YCS_YEARS).GetValues<unsigned>();

  // Ensure defined categories were valid
  for(const string& category : category_labels_) {
    if (!Categories::Instance()->IsValid(category))
      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << category << " is not a valid category");
  }

  if (age_ < model_->min_age())
    LOG_ERROR(parameters_.location(PARAM_AGE) << " (" << age_ << ") cannot be less than the model's min_age (" << model_->min_age() << ")");
  if (age_ > model_->max_age())
    LOG_ERROR(parameters_.location(PARAM_AGE) << " (" << age_ << ") cannot be greater than the model's max_age (" << model_->max_age() << ")");

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << " defined need 1 proportion defined per category. There are " << category_labels_.size()
        << " categories and " << proportions_.size() << " proportions defined.");

  RegisterAsEstimable(PARAM_PROPORTIONS, proportions_);
  Double running_total = std::accumulate(proportions_.begin(), proportions_.end(), 0.0);
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
  unsigned number_of_years = model_->final_year() - model_->start_year() + 1;
  if (ycs_values_.size() != number_of_years)
    LOG_ERROR(parameters_.location(PARAM_YCS_VALUES) << " need to be defined for every year. Expected " << number_of_years << " but got " << ycs_values_.size());

  RegisterAsEstimable(PARAM_YCS_VALUES, ycs_values_);
  for (double value : ycs_values_) {
    if (value < 0.0)
      LOG_ERROR(parameters_.location(PARAM_YCS_VALUES) << " value " << value << " cannot be less than 0.0");
  }


}

/**
 * Build the runtime relationships between this object and it's
 */
void RecruitmentBevertonHolt::DoBuild() {
  partition_        = accessor::CategoriesPtr(new accessor::Categories(category_labels_));

  if (phase_b0_label_ != "")
    phase_b0_         = initialisationphases::Manager::Instance().GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = derivedquantities::Manager::Instance().GetDerivedQuantity(ssb_);
  if (!derived_quantity_)
    LOG_ERROR(parameters_.location(PARAM_SSB) << "(" << ssb_ << ") could not be found. Have you defined it?");
}

/**
 * Reset all of the values so they're ready for an execution run
 */
void RecruitmentBevertonHolt::DoReset() {
  ssb_values_.clear();
  ycs_years_.clear();
  true_ycs_values_.clear();
  recruitment_values_.clear();

  for (unsigned i = model_->start_year(); i <= model_->final_year(); ++i)
    ycs_years_.push_back(i - ssb_offset_);

  Double mean_ycs = 0;
  for (unsigned i = 0; i < standardise_ycs_.size(); ++i) {
    for (unsigned j = 0; j < ycs_years_.size(); ++j) {
      if (ycs_years_[j] == standardise_ycs_[i]) {
        mean_ycs += ycs_values_[j];
        break;
      }
    }
  }

  mean_ycs /= standardise_ycs_.size();
  for (unsigned i = 0; i < ycs_values_.size(); ++i)
    standardise_ycs_.push_back(ycs_values_[i] / mean_ycs);
}

/**
 * Execute this process.
 */
void RecruitmentBevertonHolt::Execute() {
  Double amount_per = 0.0;

  if (model_->state() == State::kInitialise) {
    initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
    if (init_phase_manager.last_executed_phase() == phase_b0_) {
      amount_per = r0_;

    } else {
      b0_ = derived_quantity_->GetInitialisationValue(phase_b0_, derived_quantity_->GetInitialisationValuesSize(phase_b0_) - 1);
      Double ssb_ratio = b0_ == 0 ? 0 : derived_quantity_->GetValue(model_->start_year() - ssb_offset_) / b0_;
      Double true_ycs  = 1.0 * ssb_ratio / (1 - ((5 * steepness_ - 1) / (4 * steepness_) ) * (1 - ssb_ratio));
      amount_per = r0_ * true_ycs;
    }

  } else {
    /**
     * The model is not in an initialisation phase
     */
    Double ycs = standardise_ycs_[model_->current_year() - model_->start_year()];
    b0_ = derived_quantity_->GetInitialisationValue(phase_b0_, derived_quantity_->GetInitialisationValuesSize(phase_b0_) - 1);
    Double ssb_ratio = b0_ == 0 ? 0 : derived_quantity_->GetValue(model_->start_year() - ssb_offset_) / b0_;
    Double true_ycs  = ycs * ssb_ratio / (1 - ((5 * steepness_ - 1) / (4 * steepness_) ) * (1 - ssb_ratio));
    amount_per = r0_ * true_ycs;

    true_ycs_values_.push_back(true_ycs);
    recruitment_values_.push_back(amount_per);
    ssb_values_.push_back(derived_quantity_->GetValue(model_->start_year() - ssb_offset_));

    LOG_INFO("year = " << model_->current_year() << "; ycs = " << ycs << "; b0_ = " << b0_ << "; ssb_ratio = " << ssb_ratio << "; true_ycs = " << true_ycs << "; amount_per = " << amount_per);
  }

  auto iterator = partition_->Begin();
  for (unsigned i = 0; iterator != partition_->End(); ++iterator, ++i) {
    (*iterator)->data_[age_ - (*iterator)->min_age_] += amount_per * proportions_[i];
    LOG_INFO("Adding " << amount_per * proportions_[i] << " to " << (*iterator)->name_);
  }
}

} /* namespace processes */
} /* namespace isam */
