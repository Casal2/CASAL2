/**
 * @file Biomass.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 18/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Biomass.h"

#include "InitialisationPhases/Manager.h"

// namespaces
namespace niwa {
namespace derivedquantities {

/**
 * Calculate the cached value to use
 * for any interpolation
 */
void Biomass::PreExecute() {
  cache_value_ = 0.0;

  auto iterator = partition_.begin();
  // iterate over each category
  for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
    (*iterator)->UpdateMeanWeightData();

    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
      unsigned age = (*iterator)->min_age_ + j;
      cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetResult(age) * (*iterator)->mean_weight_per_[age];
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
void Biomass::Execute() {
  LOG_TRACE();
  Double value = 0.0;

  if (model_->state() == State::kInitialise) {
    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      (*iterator)->UpdateMeanWeightData();
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult(age) * (*iterator)->mean_weight_per_[age];
      }
    }

    unsigned initialisation_phase = initialisationphases::Manager::Instance().current_initialisation_phase();
    if (initialisation_values_.size() <= initialisation_phase)
      initialisation_values_.resize(initialisation_phase + 1);

    if (mean_proportion_method_)
      initialisation_values_[initialisation_phase].push_back(cache_value_ + ((value - cache_value_) * time_step_proportion_));
    else
      initialisation_values_[initialisation_phase].push_back((1 - time_step_proportion_) * cache_value_ + time_step_proportion_ * value);

  } else {
    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      (*iterator)->UpdateMeanWeightData();

      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult(age) * (*iterator)->mean_weight_per_[age];
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
