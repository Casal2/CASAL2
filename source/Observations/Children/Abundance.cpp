/**
 * @file Abundance.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Abundance.h"

#include "Catchabilities/Manager.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace observations {

namespace utils = isam::utilities;

/**
 * Default constructor
 */
Abundance::Abundance() {
  parameters_.RegisterAllowed(PARAM_CATCHABILITY);
  parameters_.RegisterAllowed(PARAM_OBS);
  parameters_.RegisterAllowed(PARAM_ERROR_VALUE);
  parameters_.RegisterAllowed(PARAM_DELTA);
  parameters_.RegisterAllowed(PARAM_PROCESS_ERROR);
}

/*
 * Destructor
 */
Abundance::~Abundance() {
}

/**
 * Validate configuration file parameters
 */
void Abundance::Validate() {
  Observation::Validate();

  CheckForRequiredParameter(PARAM_CATCHABILITY);
  CheckForRequiredParameter(PARAM_OBS);
  CheckForRequiredParameter(PARAM_ERROR_VALUE);

  catchability_label_ = parameters_.Get(PARAM_CATCHABILITY).GetValue<string>();
  delta_              = parameters_.Get(PARAM_DELTA).GetValue<double>(1e-10);
  process_error_      = parameters_.Get(PARAM_PROCESS_ERROR).GetValue<double>(0.0);

  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << process_error_ << ") cannot be less than 0.0");

  vector<string> obs  = parameters_.Get(PARAM_OBS).GetValues<string>();
  if (obs.size() % 2 != 0)
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs values must be in pairs. e.g obs <category> <value> <category> <value>");

  double value = 0.0;
  for (unsigned i = 0; i < obs.size(); i+=2) {
    if (!utils::To<double>(obs[i + 1], value))
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << obs[i + 1] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << value << " cannot be less than or equal to 0.0");

    proportions_[obs[i]] = value;
  }

  vector<string> error_values = parameters_.Get(PARAM_ERROR_VALUE).GetValues<string>();
  if (error_values.size() % 2 != 0)
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value values must be in pairs. e.g error_value <category> <value>");

  for (unsigned i = 0; i < error_values.size(); i+=2) {
    if (!utils::To<double>(error_values[i + 1], value))
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << error_values[i + 1] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << value << " cannot be less than or equal to 0.0");

    error_values_[error_values[i]] = value;
  }

  /**
   * Ensure we have an error value for every obs
   */
  map<string, Double>::iterator iter;
  for (iter = proportions_.begin(); iter != proportions_.end(); ++iter) {
    if (error_values_.find(iter->first) == error_values_.end()) {
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": No error_value has been specified for the obs category " << iter->first);
    }
  }

  /**
   * Verify that the likelihood is from the acceptable ones.
   */
  if (likelihood_type_ != PARAM_NORMAL && likelihood_type_ != PARAM_LOG_NORMAL && likelihood_type_ != PARAM_PSEUDO)
    LOG_ERROR(parameters_.location(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the Abundance observation");
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void Abundance::Build() {
  Observation::Build();

  catchability_ = catchabilities::Manager::Instance().GetCatchability(catchability_label_);
  if (!catchability_)
    LOG_ERROR(parameters_.location(PARAM_CATCHABILITY) << ": catchability " << catchability_label_ << " could not be found. Have you defined it?");

  partition_ = CategoriesPtr(new isam::partition::accessors::Categories(category_labels_));
  cached_partition_ = CachedCategoriesPtr(new isam::partition::accessors::cached::Categories(category_labels_));
}

/**
 * This method is called before any of the processes
 * in the timestep will be executed. This allows us to
 * take data from the partition that would otherwise be lost
 * once it's modified.
 *
 * In this instance we'll build the cache of our cached partition
 * accessor. This accessor will hold the partition state for us to use
 * during interpolation
 */
void Abundance::PreExecute() {
  cached_partition_->BuildCache();
}

/**
 * Run our observation to generate the score
 */
void Abundance::Execute() {
  Observation::Execute();

                  score_              = 0.0;
  double          expected_total      = 0.0; // value in the model
  vector<string>  keys;
  vector<Double>  expecteds;
  vector<Double>  observeds;
  vector<Double>  error_values;
  vector<Double>  process_errors;
  vector<Double>  scores;

  Double    selectivity_result        = 0.0;
  Double    start_value               = 0.0;
  Double    end_value                 = 0.0;
  Double    final_value               = 0.0;
  unsigned  age                       = 0;
  unsigned  i                         = 0;
  Double    error_value               = 0.0;

  // Loop through the obs
  map<string, Double>::iterator proportions_iter;
  for (proportions_iter = proportions_.begin(); proportions_iter != proportions_.end(); ++proportions_iter) {
    expected_total = 0.0;

    /**
     * Build our containers that will hold the values for the comparisons.
     * This includes getting the observed, expected values etc
     */
    auto cached_partition_iter  = cached_partition_->Begin();
    auto partition_iter         = partition_->Begin();
    for (i = 0; partition_iter != partition_->End(); cached_partition_iter++, partition_iter++, i++) {
      for (unsigned offset = 0; offset < (*partition_iter)->data_.size(); ++offset) {
        age = (*partition_iter)->min_age_ + offset;

        selectivity_result = selectivities_[i]->GetResult(age);
        start_value        = (*cached_partition_iter).data_[offset];
        end_value          = (*partition_iter)->data_[offset];
        final_value        = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * time_step_proportion_);
        else
          final_value = std::abs(start_value - end_value) * time_step_proportion_;

        expected_total += selectivity_result * final_value;
      }
    }

    /**
     * expected_total is the number of fish the model has for the category across
     */
    expected_total *= catchability_->q();
    error_value = error_values_[(*proportions_iter).first];

    // Store the values
    keys.push_back((*proportions_iter).first);
    expecteds.push_back(expected_total);
    observeds.push_back((*proportions_iter).second);
    error_values.push_back(error_value);
    process_errors.push_back(process_error_);
  }

  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(keys, observeds, expecteds, error_values, process_errors, delta_);
    for (unsigned offset = 0; offset < observeds.size(); ++offset)
      SaveComparison(keys[offset], expecteds[offset], observeds[offset], error_values[offset], 0.0);

  } else {
    score_ = 0.0;
    likelihood_->GetResult(scores, expecteds, observeds, error_values, process_errors, delta_);
    for (unsigned offset = 0; offset < scores.size(); ++offset) {
      score_ += scores[offset];
      SaveComparison(keys[offset], expecteds[offset], observeds[offset], likelihood_->AdjustErrorValue(process_errors[offset], error_values[offset]), scores[offset]);
    }
  }
}

} /* namespace priors */
} /* namespace isam */
