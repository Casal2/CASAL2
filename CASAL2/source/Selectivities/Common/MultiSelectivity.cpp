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
#include <cmath>

#include "../../AgeLengths/AgeLength.h"
#include "../../Model/Model.h"
#include "../Manager.h"

namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
MultiSelectivity::MultiSelectivity(shared_ptr<Model> model) : Selectivity(model) {
  /* clang-format off */
  parameters_.Bind<string>(PARAM_SELECTIVITY_LABELS, &selectivity_labels_, "The labels of the selectivities, one for each year", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which we want to apply the corresponding selectivity in", "");
  parameters_.Bind<string>(PARAM_DEFAULT_SELECTIVITY, &default_selectivity_label_, "The selectivity used in missing years and the projections (if not otherwise specified)", "");
  parameters_.Bind<string>(PARAM_PROJECTION_SELECTIVITY, &projection_selectivity_label_,"The selectivity used in missing projection years (defaults to the same selectivity label as 'default_selectivity')", "", "");
/* clang-format on */}

/**
 * Validate years
 *
 *
 */
void MultiSelectivity::DoValidate() {
  if (years_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITY_LABELS)
        << "number of selectivity labels supplied differs from the number of years supplied. This needs to be a one to one relationship. You provided "
        << selectivity_labels_.size() << " selectivity labels, but supplied  = " << years_.size() << " number of years.";
}

/**
 * DoBuild
 *
 * Build pointers to other selectivity classes
 *
 */
void MultiSelectivity::DoBuild() {
  // Build Selectivity pointers
  vector<unsigned> run_years        = model_->years();             // years for the run
  vector<unsigned> projection_years = model_->years_projection();  // projection years only
  vector<unsigned> all_years        = model_->years_all();         // years for the run + projection years
  default_selectivity_              = model_->managers()->selectivity()->GetSelectivity(default_selectivity_label_);
  projection_selectivity_           = model_->managers()->selectivity()->GetSelectivity(projection_selectivity_label_);

  if (!default_selectivity_)
    LOG_ERROR_P(PARAM_DEFAULT_SELECTIVITY) << ": Default selectivity label " << default_selectivity_label_ << " was not found.";
  projection_selectivity_ = model_->managers()->selectivity()->GetSelectivity(projection_selectivity_label_);

  if (projection_selectivity_label_ != "" && !projection_selectivity_)
    LOG_ERROR_P(PARAM_DEFAULT_SELECTIVITY) << ": The projection selectivity label " << projection_selectivity_label_ << " was not found.";

  // Fill in vector of selectivities with the default_selectivity_ value
  for (unsigned y_ndx = 0; y_ndx < all_years.size(); ++y_ndx) {
    selectivities_[all_years[y_ndx]] = default_selectivity_;
  }

  // Overwrite the vector of selectivities with the projection_selectivity_ value for the model projection_years, if supplied
  if (projection_selectivity_) {
    for (unsigned y_ndx = (run_years.size() + 1); y_ndx < projection_years.size(); ++y_ndx) {
      selectivities_[all_years[y_ndx]] = projection_selectivity_;
    }
  }

  // Then overwrite with the year value if user supplied for either/both run and projection years
  for (unsigned y_ndx = 0; y_ndx < all_years.size(); ++y_ndx) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(selectivity_labels_[y_ndx]);
    if (selectivity) {
      selectivities_[all_years[y_ndx]] = selectivity;
    }
  }

  LOG_MEDIUM() << "Multi-selectivity years and labels";
  LOG_MEDIUM() << "   Years Labels";
  for (unsigned y_ndx = 0; y_ndx < selectivities_.size(); ++y_ndx) {
    LOG_MEDIUM() << y_ndx << "  " << selectivities_[all_years[y_ndx]]->GetLabel();
  }
  LOG_MEDIUM() << "** end";
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
  return selectivities_[model_->current_year()]->GetAgeResult(age, age_length);
}

Double MultiSelectivity::get_value(Double value) {
  return selectivities_[model_->current_year()]->get_value(value);
}

Double MultiSelectivity::get_value(unsigned value) {
  return selectivities_[model_->current_year()]->get_value(value);
}
/**
 * GetLengthResult function
 *
 * @param length_bin_index
 * @return the MultiSelectivity value
 */
Double MultiSelectivity::GetLengthResult(unsigned length_bin_index) {
  return selectivities_[model_->current_year()]->GetLengthResult(length_bin_index);
}

} /* namespace selectivities */
} /* namespace niwa */
