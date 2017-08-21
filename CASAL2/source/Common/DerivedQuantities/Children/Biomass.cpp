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

#include "Common/InitialisationPhases/Manager.h"
#include "Common/TimeSteps/Manager.h"


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
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  LOG_FINE() << "Time step for calculating biomass = " << time_step_index;

  // iterate over each category
  for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
    //(*iterator)->UpdateMeanWeightData();

    for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
      unsigned age = (*iterator)->min_age_ + j;
      cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
      LOG_FINE() << "Biomass (Pre-execute) for category = " << (*iterator)->name_ << " age = " << age << " mean weight = " << (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];

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
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  LOG_FINE() << "Time step for calculating biomass = " << time_step_index;
  if (model_->state() == State::kInitialise) {

    auto iterator = partition_.begin();
    // iterate over each category
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      //(*iterator)->UpdateMeanWeightData();
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        LOG_FINE() << "Biomass for category = " << (*iterator)->name_ << " age = " << age << " mean weight = " << (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
      }
    }

    unsigned initialisation_phase = model_->managers().initialisation_phase()->current_initialisation_phase();
    if (initialisation_values_.size() <= initialisation_phase)
      initialisation_values_.resize(initialisation_phase + 1);

    Double b0_value = 0;

    if (time_step_proportion_ == 0.0) {
      b0_value = cache_value_;
      initialisation_values_[initialisation_phase].push_back(b0_value);
    } else if (time_step_proportion_ == 1.0) {
      b0_value = value;
      initialisation_values_[initialisation_phase].push_back(b0_value);
    } else if (mean_proportion_method_) {
      b0_value = cache_value_ + ((value - cache_value_) * time_step_proportion_);
      initialisation_values_[initialisation_phase].push_back(b0_value);
    } else {
      b0_value = pow(cache_value_, 1 - time_step_proportion_) * pow(value ,time_step_proportion_);
      initialisation_values_[initialisation_phase].push_back(b0_value);
    }

    // Store b0 or binitial on the model depending on what initialisation phase we are using
    vector<string> init_label = model_->initialisation_phases();
    InitialisationPhase* Init_phase = model_->managers().initialisation_phase()->GetInitPhase(init_label[initialisation_phase]);
    string type = Init_phase->type();
    if (type == PARAM_DERIVED || type == PARAM_ITERATIVE)
      model_->set_b0(label_, b0_value);
    if (type == PARAM_CINITIAL)
      model_->set_binitial(label_, b0_value);

  } else {
    auto iterator = partition_.begin();
    // iterate over each category
    LOG_FINEST() << "Partition size = " << partition_.size();
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      //(*iterator)->UpdateMeanWeightData();

      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        value += (*iterator)->data_[j] * selectivities_[i]->GetResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
      }
    }

    if (time_step_proportion_ == 0.0)
      values_[model_->current_year()] = cache_value_;
    else if (time_step_proportion_ == 1.0)
      values_[model_->current_year()] = value;
    if (mean_proportion_method_)
      values_[model_->current_year()] = cache_value_ + ((value - cache_value_) * time_step_proportion_);
    else
      values_[model_->current_year()] = pow(cache_value_, 1 - time_step_proportion_) * pow(value ,time_step_proportion_);
  }
  LOG_FINEST() << " Pre Exploitation value " <<  cache_value_ << " Post exploitation " << value << " Final value " << values_[model_->current_year()];
}

} /* namespace derivedquantities */
} /* namespace niwa */
