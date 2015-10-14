/**
 * @file Abundance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Abundance.h"

#include "InitialisationPhases/Manager.h"

// namespaces
namespace niwa {
namespace derivedquantities {


/**
 * PreExecute version of Execute
 */
void Abundance::PreExecute() {
  cache_value_ = 0.0;

  auto iterator = partition_.begin();
  // iterate over each category
  for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
      cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
    }
  }
}
/**
 * Calculate the derived quantity value for the
 * state of the model.
 *
 * This class will calculate a value that is the sum total
 * of the population in the model filtered by category and
 * multiplied by the selectivities.
 *
 */
void Abundance::Execute() {
  Double value = 0.0;

  if (model_->state() == State::kInitialise) {
    // initialisation calculation
    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
      }
    }

    unsigned initialisation_phase = model_->managers().initialisation_phase()->current_initialisation_phase();
    if (initialisation_values_.size() <= initialisation_phase)
      initialisation_values_.resize(initialisation_phase + 1);

    if (mean_proportion_method_)
      initialisation_values_[initialisation_phase].push_back(cache_value_ + ((value - cache_value_) * time_step_proportion_));
    else
      initialisation_values_[initialisation_phase].push_back((1 - time_step_proportion_) * cache_value_ + time_step_proportion_ * value);

  } else {
    // execution calculation
    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
      }
    }

    if (mean_proportion_method_)
      values_[model_->current_year()] = cache_value_ + ((value - cache_value_) * time_step_proportion_);
    else
      values_[model_->current_year()] = (1 - time_step_proportion_) * cache_value_ + time_step_proportion_ * value;
  }
}

} /* namespace derivedquantities */
} /* namespace niwa */
