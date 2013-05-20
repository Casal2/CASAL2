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

  // Delta
  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << AS_DOUBLE(delta_) << ") cannot be less than 0.0");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << AS_DOUBLE(process_error_) << ") cannot be less than 0.0");

  // Obs
  vector<string> obs  = parameters_.Get(PARAM_OBS).GetValues<string>();
  if (obs.size() != category_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs values length (" << obs.size() << ") must match the number of category collections provided ("
        << category_labels_.size() << ")");

  double value = 0.0;
  for (unsigned i = 0; i < obs.size(); ++i) {
    if (!utils::To<double>(obs[i], value))
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << obs[i] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << value << " cannot be less than or equal to 0.0");

    proportions_.push_back(value);
  }

  // Error Value
  vector<string> error_values = parameters_.Get(PARAM_ERROR_VALUE).GetValues<string>();
  if (error_values.size() != obs.size())
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value length (" << error_values.size()
        << ") must be same length as obs (" << obs.size() << ")");

  for (unsigned i = 0; i < error_values.size(); ++i) {
    if (!utils::To<double>(error_values[i], value))
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << error_values[i] << " cannot be converted to a double");
    if (value <= 0.0)
      LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value " << value << " cannot be less than or equal to 0.0");

    error_values_.push_back(value);
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

  partition_ = CombinedCategoriesPtr(new isam::partition::accessors::CombinedCategories(category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new isam::partition::accessors::cached::CombinedCategories(category_labels_));
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
                  score_              = 0.0;
  Double          expected_total      = 0.0; // value in the model
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
  Double    error_value               = 0.0;

  // Loop through the obs
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // auto = map<map<string, vector<partition::category&> > >

  if (cached_partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("cached_partition_->Size() != proportions_.size()");
  if (partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("partition_->Size() != proportions_.size()");

  for (unsigned category_offset = 0; category_offset < proportions_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    expected_total = 0.0;

    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {

      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        age = (*category_iter)->min_age_ + data_offset;

        selectivity_result = selectivities_[category_offset]->GetResult(age);
        start_value        = (*cached_category_iter).data_[data_offset];
        end_value          = (*category_iter)->data_[data_offset];
        final_value        = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * time_step_proportion_);
        else {
          // re-write of std::abs(start_value - end_value) * temp_step_proportion for ADMB
          Double temp = start_value - end_value;
          temp = temp < 0 ? temp : temp * -1.0;
          final_value = temp * time_step_proportion_;
        }

        expected_total += selectivity_result * final_value;
      }
    }

    /**
     * expected_total is the number of fish the model has for the category across
     */
    expected_total *= catchability_->q();
    error_value = error_values_[category_offset];

    // Store the values
    keys.push_back(category_labels_[category_offset]);
    expecteds.push_back(expected_total);
    observeds.push_back(proportions_[category_offset]);
    error_values.push_back(error_value);
    process_errors.push_back(process_error_);
  }

  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(keys, observeds, expecteds, error_values, process_errors, delta_);
    for (unsigned index = 0; index < observeds.size(); ++index)
      SaveComparison(keys[index], expecteds[index], observeds[index], error_values[index], 0.0);

  } else {
    score_ = 0.0;
    likelihood_->GetResult(scores, expecteds, observeds, error_values, process_errors, delta_);
    for (unsigned index = 0; index < scores.size(); ++index) {
      score_ += scores[index];
      SaveComparison(keys[index], expecteds[index], observeds[index], likelihood_->AdjustErrorValue(process_errors[index], error_values[index]), scores[index]);
    }
  }
}

} /* namespace priors */
} /* namespace isam */
