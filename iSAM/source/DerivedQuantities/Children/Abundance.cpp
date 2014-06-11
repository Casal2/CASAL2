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

// namespaces
namespace isam {
namespace derivedquantities {

/**
 * Calculate the derived quantity value for the kExecute
 * state of the model.
 *
 * This class will calculate a value that is the sum total
 * of the population in the model filtered by category and
 * multiplied by the selectivities.
 *
 */
void Abundance::Calculate() {
  Double value = 0.0;

  auto iterator = partition_.begin();
  // iterate over each category
  for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
      value += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
    }
  }

  values_[model_->current_year()] = value;
}

/**
 * Calculate the value for this derived quantity during the
 * initialisation phase and store it based on the phase we're in
 */
void Abundance::Calculate(unsigned initialisation_phase) {
  LOG_TRACE();
  Double value = 0.0;

  auto iterator = partition_.begin();
  // iterate over each category
  for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
      value += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
    }
  }

  if (initialisation_values_.size() <= initialisation_phase)
    initialisation_values_.resize(initialisation_phase + 1);

  initialisation_values_[initialisation_phase].push_back(value);
}


} /* namespace derivedquantities */
} /* namespace isam */
