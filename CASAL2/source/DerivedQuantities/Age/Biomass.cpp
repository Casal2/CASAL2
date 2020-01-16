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
#include "DerivedQuantities/Age/Biomass.h"
#include "InitialisationPhases/Manager.h"
#include "TimeSteps/Manager.h"
#include "AgeWeights/Manager.h"

// namespaces
namespace niwa {
namespace derivedquantities {
namespace age {

Biomass::Biomass(Model* model) : DerivedQuantity(model) {
  parameters_.Bind<string>(PARAM_AGE_WEIGHT_LABELS, &age_weight_labels_, "The labels for the @age_weight block which corresponds to each category, to use that weight calculation method for biomass calculations", "", "");

}

/**
 * Validate class
 */
void Biomass::DoValidate() {
  if (parameters_.Get(PARAM_AGE_WEIGHT_LABELS)->has_been_defined()) {
    // Do some house keeping if this parameter has been defined
    if(age_weight_labels_.size() != category_labels_.size())
      LOG_ERROR_P(PARAM_AGE_WEIGHT_LABELS) << "If age_weight_labels are used, one is required for each category. There are "
        << age_weight_labels_.size() << " age weight labels, but there are " << category_labels_.size() << " category labels";
  }
}


/**
 * Build pointers class
 */
void Biomass::DoBuild() {
  // Build age weight pointers if users have define it
  if (parameters_.Get(PARAM_AGE_WEIGHT_LABELS)->has_been_defined()) {
    use_age_weights_ = true;
    LOG_FINE() << "Age weight has been defined";
    for (string label : age_weight_labels_) {
      AgeWeight* age_weight = model_->managers().age_weight()->FindAgeWeight(label);
      if (!age_weight)
        LOG_ERROR_P(PARAM_AGE_WEIGHT_LABELS) << "Age-weight label (" << label << ") was not found.";
      age_weights_.push_back(age_weight);
    }
  }
}

/**
 * Calculate the cached value to use
 * for any interpolation
 */
void Biomass::PreExecute() {
  cache_value_ = 0.0;
  unsigned year = model_->current_year();
  auto iterator = partition_.begin();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  LOG_FINE() << "Time step for calculating biomass = " << time_step_index;

  // iterate over each category
  if (!use_age_weights_) {
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      (*iterator)->UpdateMeanWeightData();
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
        LOG_FINE() << "Biomass (Pre-execute) for category = " << (*iterator)->name_ << " age = " << age
          << " mean weight = " << (*iterator)->mean_weight_by_time_step_age_[time_step_index][age]
          << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
      }
    }
  } else {
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      (*iterator)->UpdateMeanWeightData();
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        LOG_FINE() << "Biomass (Pre-execute) for category = " << (*iterator)->name_ << " age = " << age
          << " mean weight = " << age_weights_[i]->mean_weight_at_age_by_year(year, age)
          << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];

      }
    }
  }

  LOG_TRACE();
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
  unsigned year = model_->current_year();
  Double value = 0.0;
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  LOG_FINE() << "Time step for calculating biomass = " << time_step_index;
  if (model_->state() == State::kInitialise) {
    auto iterator = partition_.begin();
    if (!use_age_weights_) {
      // iterate over each category
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        (*iterator)->UpdateMeanWeightData();
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          LOG_FINE() << "Biomass for category = " << (*iterator)->name_ << " age = " << age
            << " mean weight = " << (*iterator)->mean_weight_by_time_step_age_[time_step_index][age]
            << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
        }
      }
    } else {
      // iterate over each category
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        (*iterator)->UpdateMeanWeightData();
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          LOG_FINE() << "Biomass for category = " << (*iterator)->name_ << " age = " << age
            << " mean weight = " << age_weights_[i]->mean_weight_at_age_by_year(year, age)
            << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        }
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
    if (!use_age_weights_) {
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        (*iterator)->UpdateMeanWeightData();
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
        }
      }
    } else {
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        (*iterator)->UpdateMeanWeightData();
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        }
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
  LOG_FINEST() << " Pre Exploitation value " <<  cache_value_ << " Post exploitation " << value
    << " Final value " << values_[model_->current_year()];
}

} /* namespace age */
} /* namespace derivedquantities */
} /* namespace niwa */
