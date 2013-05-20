/**
 * @file ProportionsAtAge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ProportionsAtAge.h"

#include <algorithm>

#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Default constructor
 */
ProportionsAtAge::ProportionsAtAge() {
  parameters_.RegisterAllowed(PARAM_MIN_AGE);
  parameters_.RegisterAllowed(PARAM_MAX_AGE);
  parameters_.RegisterAllowed(PARAM_AGE_PLUS);
  parameters_.RegisterAllowed(PARAM_TOLERANCE);
  parameters_.RegisterAllowed(PARAM_OBS);
  parameters_.RegisterAllowed(PARAM_ERROR_VALUE);
  parameters_.RegisterAllowed(PARAM_DELTA);
  parameters_.RegisterAllowed(PARAM_PROCESS_ERROR);
  parameters_.RegisterAllowed(PARAM_AGEING_ERROR);
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAge::Validate() {
  Observation::Validate();

  CheckForRequiredParameter(PARAM_MIN_AGE);
  CheckForRequiredParameter(PARAM_MAX_AGE);
  CheckForRequiredParameter(PARAM_OBS);

  min_age_            = parameters_.Get(PARAM_MIN_AGE).GetValue<unsigned>();
  max_age_            = parameters_.Get(PARAM_MAX_AGE).GetValue<unsigned>();
  age_plus_           = parameters_.Get(PARAM_AGE_PLUS).GetValue<bool>(true);
  delta_              = parameters_.Get(PARAM_DELTA).GetValue<double>(DELTA);
  tolerance_          = parameters_.Get(PARAM_TOLERANCE).GetValue<double>(0.001);
  process_error_      = parameters_.Get(PARAM_PROCESS_ERROR).GetValue<double>(0.0);
  ageing_error_label_ = parameters_.Get(PARAM_AGEING_ERROR).GetValue<string>("");
  error_values_       = parameters_.Get(PARAM_ERROR_VALUE).GetValues<double, Double>();
  age_spread_         = (max_age_ - min_age_) + 1;

  /**
   * Do some simple checks
   */
  ModelPtr model = Model::Instance();
  if (min_age_ < model->min_age())
    LOG_ERROR(parameters_.location(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model->min_age() << ")");
  if (max_age_ > model->max_age())
    LOG_ERROR(parameters_.location(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model->max_age() << ")");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << AS_DOUBLE(process_error_) << ") cannot be less than 0.0");
  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << AS_DOUBLE(delta_) << ") cannot be less than 0.0");
  if (error_values_.size() != category_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": number error_values provided (" << error_values_.size() << ") does not match the number of "
        << "category collections provided (" << category_labels_.size() << ")");

  /**
   * Ensure error values are consistent for different likelihood types
   * Note: use of C++ lamba functions
   */
  if (likelihood_type_ == PARAM_LOG_NORMAL) {
    std::for_each(std::begin(error_values_), std::end(error_values_), [&](Double n) {
      if (n <= 0.0)
        LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value (" << AS_DOUBLE(n) << ") cannot be equal to or less than 0.0");
    });

  } else if (likelihood_type_ == PARAM_MULTINOMIAL) {
    std::for_each(std::begin(error_values_), std::end(error_values_), [&](Double n) {
      if (n < 0.0)
        LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value (" << AS_DOUBLE(n) << ") cannot be less than 0.0");
    });

  } else
    LOG_ERROR(parameters_.location(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the proportions at age observation. "
        << "Supported types are " << PARAM_LOG_NORMAL << " and " << PARAM_MULTINOMIAL);

  /**
   * Validate the number of obs provided matches age spread * category_labels.
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  vector<string> obs;
  obs = parameters_.Get(PARAM_OBS).GetValues<string>();
  unsigned obs_expected = age_spread_ * category_labels_.size();
  if (obs.size() != obs_expected)
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": number of obs provided (" << obs.size() << ") does not match the number expected (" << obs_expected
        << "). Number expected is the age spread (" << age_spread_ << ") * category collections (" << category_labels_.size() << ")");

  /**
   * Populate our proportions matrix.
   *
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  proportions_.resize(category_labels_.size());
  double value = 0.0;
  double total = 0.0;
  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    total = 0.0;
    for (unsigned j = 0; j < age_spread_; ++j) {
      unsigned obs_index = i * age_spread_ + j;
      if (!utilities::To<double>(obs[obs_index], value))
        LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value (" << obs[obs_index] << ") at index " << obs_index + 1
            << " in the definition could not be converted to a numeric double");

      proportions_[i].push_back(value);
      total += value;
    }

    if (fabs(1.0 - total) > tolerance_) {
      LOG_WARNING(parameters_.location(PARAM_OBS) << ": obs total (" << total << ") exceeds tolerance for comparison to 1.0 for category collection "
          << category_labels_[i] << ". Auto-scaling proportions to 1.0");

      for (unsigned j = 0; j < proportions_[i].size(); ++j)
        proportions_[i][j] /= total;
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProportionsAtAge::Build() {
  Observation::Build();

  partition_ = CombinedCategoriesPtr(new isam::partition::accessors::CombinedCategories(category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new isam::partition::accessors::cached::CombinedCategories(category_labels_));

  if (ageing_error_label_ != "")
    LOG_CODE_ERROR("ageing error has not been implemented for the proportions at age observation");

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);
}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProportionsAtAge::PreExecute() {
  cached_partition_->BuildCache();
}

/**
 *
 */
void ProportionsAtAge::Execute() {
  LOG_TRACE();

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // auto = map<map<string, vector<partition::category&> > >

  if (cached_partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("cached_partition_->Size() != proportions_.size()");
  if (partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("partition_->Size() != proportions_.size()");

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  Double      running_total      = 0.0;
  Double      selectivity_result = 0.0;
  Double      start_value        = 0.0;
  Double      end_value          = 0.0;
  Double      final_value        = 0.0;

  vector<string>    keys;
  vector<unsigned>  ages;
  vector<Double>    expecteds;
  vector<Double>    observeds;
  vector<Double>    error_values;
  vector<Double>    process_errors;
  vector<Double>    scores;

  for (unsigned category_offset = 0; category_offset < proportions_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    vector<Double> expected_values(age_spread_, 0.0);
    running_total = 0.0;

    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */
    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {

      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        if ((*category_iter)->min_age_ + data_offset < min_age_)
          continue;
        if ((*category_iter)->min_age_ + data_offset > max_age_ && !age_plus_)
          break;

        unsigned age_offset = ( (*category_iter)->min_age_ + data_offset) - min_age_;
        unsigned age        = ( (*category_iter)->min_age_ + data_offset);

        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;

//        LOG_INFO("age: " << age << "; age_offset: " << age_offset << "; age_spread: " << age_spread_ << "; cat->min_age_: " << (*category_iter)->min_age_);

        selectivity_result = selectivities_[category_offset]->GetResult(age);
        start_value   = (*cached_category_iter).data_[data_offset];
        end_value     = (*category_iter)->data_[data_offset];
        final_value   = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * time_step_proportion_);
        else
          final_value = fabs(start_value - end_value) * time_step_proportion_;

        expected_values[age_offset] += final_value * selectivity_result;
        running_total += final_value * selectivity_result;
      }
    }
//    LOG_INFO("running total for categories " << category_labels_[category_offset] << " = " << running_total);

    if (expected_values.size() != proportions_[category_offset].size())
      LOG_CODE_ERROR("expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size(" << proportions_[category_offset].size() << ")");

    /**
     * Convert the expected_values in to a proportion
     */
    for (Double& value : expected_values)
      value = value / running_total;
//    LOG_INFO("Finished converting values to a proportion");

    for (unsigned i = 0; i < expected_values.size(); ++i) {
      keys.push_back(category_labels_[category_offset]);
      ages.push_back(min_age_ + i);
      expecteds.push_back(expected_values[i]);
      observeds.push_back(proportions_[category_offset][i]);
      error_values.push_back(error_values_[category_offset]);
      process_errors.push_back(process_error_);
    }

    if (expected_values.size() != observeds.size())
      LOG_CODE_ERROR("expected_values.size(" << expected_values.size() << ") != observeds.size(" << observeds.size() << ")");
  }

  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(keys, observeds, expecteds, error_values, process_errors, delta_);
    for (unsigned index = 0; index < observeds.size(); ++index)
      SaveComparison(keys[index], ages[index], expecteds[index], observeds[index], error_values[index], 0.0);

  } else {
    score_ = likelihood_->GetInitialScore(keys, process_errors, error_values);

    likelihood_->GetResult(scores, expecteds, observeds, error_values, process_errors, delta_);
    for (unsigned index = 0; index < scores.size(); ++index) {
      score_ += scores[index];
      SaveComparison(keys[index], ages[index], expecteds[index], observeds[index], likelihood_->AdjustErrorValue(process_errors[index], error_values[index]), scores[index]);
    }
  }

}

} /* namespace observations */
} /* namespace isam */
