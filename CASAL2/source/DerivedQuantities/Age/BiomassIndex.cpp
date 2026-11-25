/**
 * @file BiomassIndex.cpp
 * @author  A Dunn
 * @date 01/04/2024
 * @section LICENSE
 */

// headers
#include "../../DerivedQuantities/Age/BiomassIndex.h"

#include "../../AgeLengths/AgeLength.h"
#include "../../AgeWeights/Manager.h"
#include "../../Catchabilities/Common/Nuisance.h"
#include "../../Catchabilities/Manager.h"
#include "../../InitialisationPhases/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace math = niwa::utilities::math;
namespace niwa {
namespace derivedquantities {
namespace age {

/**
 * Default constructor
 */
BiomassIndex::BiomassIndex(shared_ptr<Model> model) : DerivedQuantity(model) {
  // clang-format off
  parameters_.Bind<string>(PARAM_AGE_WEIGHT_LABELS, &age_weight_labels_, "The labels for the age-weights that correspond to each category for the biomass_index calculation")
    ->set_default_value("");
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "The type of distribution for the biomass index")->set_default_value(PARAM_LOGNORMAL);
  parameters_.Bind<double>(PARAM_CV, &cv_, "The cv for the uncertainty for the distribution when generating the biomass index")->set_default_value(0.2);
  parameters_.Bind<double>(PARAM_BIAS, &bias_, "The bias (a positive or negative proportion) when generating the biomass index")->set_default_value(0.0);
  parameters_.Bind<double>(PARAM_RHO, &rho_, "The autocorrelation between annual values when generating the biomass index")->set_default_value(0.0);
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The catchability to use when generating the biomass index")->set_default_value("");
  // clang-format on
}

/**
 * Validate class
 */
void BiomassIndex::DoValidate() {
  parameters_.Validate(PARAM_DISTRIBUTION)->IsInList({PARAM_NORMAL, PARAM_LOGNORMAL, PARAM_NONE});
  parameters_.Validate(PARAM_CV)->GreaterThanOrEqualTo(0.0);
  parameters_.Validate(PARAM_RHO)->GreaterThanOrEqualTo(0.0);

  if (parameters_.Get(PARAM_AGE_WEIGHT_LABELS)->has_been_defined()) {
    // Do some house keeping if this parameter has been defined
    if (age_weight_labels_.size() != category_labels_.size())
      LOG_ERROR_P(PARAM_AGE_WEIGHT_LABELS) << "If age_weight_labels are used, one is required for each category. There are " << age_weight_labels_.size()
                                           << " age_weight_labels, but there are " << category_labels_.size() << " category_labels";
  }
  if (distribution_ != PARAM_NONE && cv_ <= 0) {
    LOG_ERROR_P(PARAM_CV) << "if the " PARAM_DISTRIBUTION << " is " << PARAM_NORMAL << " or " << PARAM_LOGNORMAL << ", then the " << PARAM_CV << " must be greater than 0";
  }
}

/**
 * Build pointers class
 */
void BiomassIndex::DoBuild() {
  // Build age weight pointers if users have define it
  if (parameters_.Get(PARAM_AGE_WEIGHT_LABELS)->has_been_defined()) {
    use_age_weights_ = true;
    LOG_FINE() << "Age-weight has been defined";
    for (string label : age_weight_labels_) {
      AgeWeight* age_weight = model()->managers()->age_weight()->FindAgeWeight(label);
      if (!age_weight)
        LOG_ERROR_P(PARAM_AGE_WEIGHT_LABELS) << "Age-weight label (" << label << ") was not found.";
      age_weights_.push_back(age_weight);
    }
  }
  if (distribution_ == PARAM_LOGNORMAL) {
    sigma_                      = sqrt(cv_ * cv_ + 1.0);
    bias_adjustment_multiplier_ = 1.0 / (cv_ * cv_ + 1);
  } else if (distribution_ == PARAM_NORMAL) {
    sigma_                      = cv_;
    bias_adjustment_multiplier_ = 1.0;
  } else if (distribution_ == PARAM_NONE) {
    sigma_                      = 0.0;
    bias_adjustment_multiplier_ = 1.0;
  } else {
    LOG_ERROR_P(PARAM_DISTRIBUTION) << "is not a valid distribution type";
  }
  if (catchability_label_ != "") {
  } else {
    catchability_value_ = 1.0;
  }
  catchability_ = model()->managers()->catchability()->GetCatchability(catchability_label_);
  if (!catchability_)
    catchability_value_ = 1.0;

  biomass_      = 0.0;
  last_biomass_ = 0.0;
}

/**
 * Calculate the cached value to use
 * for any interpolation
 */
void BiomassIndex::PreExecute() {
  cache_value_             = 0.0;
  unsigned year            = model()->current_year();
  auto     iterator        = partition_.begin();
  unsigned time_step_index = model()->managers()->time_step()->current_time_step();
  LOG_FINE() << "The time-step when calculating biomass = " << time_step_index;

  // iterate over each category
  if (!use_age_weights_) {
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->age_length_->mean_weight(time_step_index, age);
        LOG_FINEST() << "Biomass (Pre-execute) for category = " << (*iterator)->name_ << " age = " << age
                     << " mean weight = " << (*iterator)->age_length_->mean_weight(time_step_index, age)
                     << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
      }
    }
  } else {
    for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
      for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
        unsigned age = (*iterator)->min_age_ + j;
        cache_value_ += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        LOG_FINEST() << "Biomass (Pre-execute) for category = " << (*iterator)->name_ << " age = " << age
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
void BiomassIndex::Execute() {
  LOG_TRACE();
  unsigned year            = model()->current_year();
  Double   value           = 0.0;
  unsigned time_step_index = model()->managers()->time_step()->current_time_step();
  LOG_FINE() << "Time step for calculating biomass = " << time_step_index;

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  double rng_value = 0;
  if (distribution_ == PARAM_LOGNORMAL) {
    rng_value = rng.lognormal(1.0, cv_) * (1.0 + bias_);
  } else if (distribution_ == PARAM_NORMAL) {
    rng_value = rng.normal(1.0, cv_) * (1.0 + bias_);
  } else if (distribution_ == PARAM_NONE) {
    rng_value = 1.0 + bias_;
  } else {
    LOG_ERROR_P(PARAM_DISTRIBUTION) << "is not a valid distribution type";
  }

  if (catchability_) {
    catchability_value_ = catchability_->q();
  } else {
    catchability_value_ = 1.0;
  }

  if (model()->state() == State::kInitialise) {
    auto iterator = partition_.begin();
    LOG_FINEST() << "Partition size = " << partition_.size();
    if (!use_age_weights_) {
      // iterate over each category
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          LOG_FINEST() << "Biomass for category = " << (*iterator)->name_ << " age = " << age << " mean weight = " << (*iterator)->age_length_->mean_weight(time_step_index, age)
                       << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->age_length_->mean_weight(time_step_index, age);
        }
      }
    } else {
      // iterate over each category
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          LOG_FINEST() << "Biomass for category = " << (*iterator)->name_ << " age = " << age << " mean weight = " << age_weights_[i]->mean_weight_at_age_by_year(year, age)
                       << " selectivity = " << selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) << " numbers = " << (*iterator)->data_[j];
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        }
      }
    }

    unsigned initialisation_phase = model()->managers()->initialisation_phase()->current_initialisation_phase();
    if (initialisation_values_.size() <= initialisation_phase)
      initialisation_values_.resize(initialisation_phase + 1);

    if (time_step_proportion_ == 0.0) {
      biomass_ = cache_value_ * rng_value;
      biomass_ = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_ = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      initialisation_values_[initialisation_phase].push_back(biomass_);
    } else if (time_step_proportion_ == 1.0) {
      biomass_ = value * rng_value;
      biomass_ = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_ = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      initialisation_values_[initialisation_phase].push_back(biomass_);
    } else if (mean_proportion_method_) {
      Double temp = cache_value_ + ((value - cache_value_) * time_step_proportion_);
      biomass_    = temp * rng_value;
      biomass_    = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_    = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      initialisation_values_[initialisation_phase].push_back(biomass_);
    } else {
      Double temp = pow(cache_value_, 1 - time_step_proportion_) * pow(value, time_step_proportion_);
      biomass_    = temp * rng_value;
      biomass_    = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_    = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      initialisation_values_[initialisation_phase].push_back(biomass_);
    }

  } else {
    auto iterator = partition_.begin();
    // iterate over each category
    LOG_FINEST() << "Partition size = " << partition_.size();
    if (!use_age_weights_) {
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * (*iterator)->age_length_->mean_weight(time_step_index, age);
        }
      }
    } else {
      for (unsigned i = 0; i < partition_.size() && iterator != partition_.end(); ++i, ++iterator) {
        for (unsigned j = 0; j < (*iterator)->data_.size(); ++j) {
          unsigned age = (*iterator)->min_age_ + j;
          value += (*iterator)->data_[j] * selectivities_[i]->GetAgeResult(age, (*iterator)->age_length_) * age_weights_[i]->mean_weight_at_age_by_year(year, age);
        }
      }
    }

    if (time_step_proportion_ == 0.0) {
      biomass_                         = cache_value_ * rng_value;
      biomass_                         = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_                         = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      values_[model()->current_year()] = biomass_;
    } else if (time_step_proportion_ == 1.0) {
      biomass_                         = value * rng_value;
      biomass_                         = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_                         = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      values_[model()->current_year()] = biomass_;
    } else if (mean_proportion_method_) {
      Double temp                      = cache_value_ + ((value - cache_value_) * time_step_proportion_);
      biomass_                         = temp * rng_value;
      biomass_                         = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_                         = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      values_[model()->current_year()] = biomass_;
    } else {
      Double temp                      = pow(cache_value_, 1 - time_step_proportion_) * pow(value, time_step_proportion_);
      biomass_                         = temp * rng_value;
      biomass_                         = (rho_ * last_biomass_) + ((1.0 - rho_) * biomass_);
      biomass_                         = math::ZeroFun(biomass_, math::ZERO) * catchability_value_;
      values_[model()->current_year()] = biomass_;
    }
  }
  last_biomass_ = biomass_;
  LOG_FINE() << " Pre Exploitation value " << cache_value_ << " time step proportion " << time_step_proportion_ << " Post exploitation " << value << " Final value "
             << values_[model()->current_year()];
}

} /* namespace age */
} /* namespace derivedquantities */
} /* namespace niwa */
