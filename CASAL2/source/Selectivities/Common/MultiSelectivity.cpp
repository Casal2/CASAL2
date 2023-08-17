/**
 * @file MultiSelectivity.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 16/08/2023
 * @section LICENSE
 *
 * Copyright NIWA Science 2023 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * 
 */
#include "MultiSelectivity.h"

#include <boost/math/distributions/lognormal.hpp>
#include "../Manager.h"

#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"

namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
MultiSelectivity::MultiSelectivity(shared_ptr<Model> model) : Selectivity(model) {
  parameters_.Bind<string>(PARAM_SELECTIVITY_LABELS, &selectivity_labels_, "The labels of the selectivities, one for each year", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which we want to apply the corresponding selectivity in", "");
  parameters_.Bind<string>(PARAM_DEFAULT_SELECTIVITY, &default_selectivity_label_, "The selectivity used in missing years", "");

}

/**
 * Validate years
 * 
 *
 */
void MultiSelectivity::DoValidate() {
   if(years_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITY_LABELS) << "number of selectivity labels supplied differs from the number of years supplied. This needs to be a one to one relationship. You provided " << selectivity_labels_.size() << " selectivity labels, but supplied  = " << years_.size() << " number of years.";
}

/**
 * DoBuild
 * 
 * Build pointers to other selectivity classes
 *
 */
void MultiSelectivity::DoBuild() {
  // Build Selectivity pointers
  vector<unsigned> model_years = model_->years_all();
  default_selectivity_ = model_->managers()->selectivity()->GetSelectivity(default_selectivity_label_);
    if (!default_selectivity_)
      LOG_ERROR_P(PARAM_DEFAULT_SELECTIVITY) << ": Default selectivity label " << default_selectivity_label_ << " was not found.";

  for(unsigned y_ndx = 0; y_ndx < years_.size(); ++y_ndx) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(selectivity_labels_[y_ndx]);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_SELECTIVITY_LABELS) << ": selectivity label " << selectivity_labels_[y_ndx] << " was not found.";
    } else {
      selectivities_[years_[y_ndx]] = selectivity;
    }
  }
}

/**
 * Return the MultiSelectivity result regardless of the
 * age or length specified
 *
 * @param age (unused in this selectivity)
 * @param age_or_length AgeLength pointer (unused in this selectivity)
 * @return the MultiSelectivity value
 */
Double MultiSelectivity::GetAgeResult(unsigned age, AgeLength* age_length) {
  if(find(years_.begin(), years_.end(), model_->current_year()) == years_.end())
    default_selectivity_->GetAgeResult(age, age_length);
  return selectivities_[model_->current_year() - model_->start_year()]->GetAgeResult(age, age_length);
}

Double MultiSelectivity::get_value(Double value) {
  if(find(years_.begin(), years_.end(), model_->current_year()) == years_.end())
    default_selectivity_->get_value(value);
  return selectivities_[model_->current_year()]->get_value(value);
}

Double MultiSelectivity::get_value(unsigned value) {
  if(find(years_.begin(), years_.end(), model_->current_year()) == years_.end())
    default_selectivity_->get_value(value);
  return selectivities_[model_->current_year()]->get_value(value);
}
/**
 * GetLengthResult function
 *
 * @param length_bin_index
 * @return the MultiSelectivity value
 */
Double MultiSelectivity::GetLengthResult(unsigned length_bin_index) {
  if(find(years_.begin(), years_.end(), model_->current_year()) == years_.end())
    default_selectivity_->GetLengthResult(length_bin_index);  
  return selectivities_[model_->current_year()]->GetLengthResult(length_bin_index);
}

} /* namespace selectivities */
} /* namespace niwa */
