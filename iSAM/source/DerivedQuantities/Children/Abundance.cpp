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

    unsigned initialisation_phase = initialisationphases::Manager::Instance().current_initialisation_phase();
    if (initialisation_values_.size() <= initialisation_phase)
      initialisation_values_.resize(initialisation_phase + 1);

    initialisation_values_[initialisation_phase].push_back(value);
  } else {
    // execution calculation
    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult((*iterator)->min_age_ + j);
      }
    }

    values_[model_->current_year()] = value;
  }
}

} /* namespace derivedquantities */
} /* namespace niwa */
