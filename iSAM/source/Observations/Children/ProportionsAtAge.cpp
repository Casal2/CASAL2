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
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Use age plus group", true);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "Tolerance", 0.001);
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "Year to execute in");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values");
  parameters_.Bind<double>(PARAM_ERROR_VALUE, &error_values_, "Error values");
  parameters_.Bind<double>(PARAM_DELTA, &delta_, "Delta", DELTA);
  parameters_.Bind<double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", 0.0);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "");
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAge::DoValidate() {
  age_spread_ = (max_age_ - min_age_) + 1;

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

  if (error_values_.size() == 1)
    error_values_.resize(obs_.size(), error_values_[0]);
  if (error_values_.size() != obs_.size())
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": number error_values provided (" << error_values_.size() << ") does not match the number of "
        << "observations provided (" << obs_.size() << ")");

  /**
   * Ensure error values are consistent for different likelihood types
   * Note: use of C++ lamba functions
   */
  if (likelihood_type_ == PARAM_LOGNORMAL) {
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
        << "Supported types are " << PARAM_LOGNORMAL << " and " << PARAM_MULTINOMIAL);

  /**
   * Validate the number of obs provided matches age spread * category_labels.
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned obs_expected = age_spread_ * category_labels_.size();
  if (obs_.size() != obs_expected)
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": number of obs_ provided (" << obs_.size() << ") does not match the number expected (" << obs_expected
        << "). Number expected is the age spread (" << age_spread_ << ") * category collections (" << category_labels_.size() << ")");

  /**
   * Populate our proportions matrix.
   *
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  proportions_.resize(category_labels_.size());
  error_values_by_category_.resize(category_labels_.size());
  double value = 0.0;
  double total = 0.0;
  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    total = 0.0;
    for (unsigned j = 0; j < age_spread_; ++j) {
      unsigned obs_index = i * age_spread_ + j;
      if (!utilities::To<double>(obs_[obs_index], value))
        LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs_ value (" << obs_[obs_index] << ") at index " << obs_index + 1
            << " in the definition could not be converted to a numeric double");

      proportions_[i].push_back(value);
      error_values_by_category_[i].push_back( error_values_[i*age_spread_ + j]);
      total += value;
    }

    // TODO: THIS MUST BE RE_PROPORTIONED SO THE TOTAL OBS SUM TO 1. NOT DISTINCT BY CATEGORY
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
void ProportionsAtAge::DoBuild() {
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

  if (cached_partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("cached_partition_->Size() != proportions_.size()");
  if (partition_->Size() != proportions_.size())
    LOG_CODE_ERROR("partition_->Size() != proportions_.size()");
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
  auto partition_iter         = partition_->Begin(); // auto = map<map<string(category), vector<partition::category&> > >



  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset) {
    Double      running_total      = 0.0;
    Double      selectivity_result = 0.0;
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;
    Double      final_value        = 0.0;

    for (unsigned proportions_offset = 0; proportions_offset < proportions_.size(); ++proportions_offset, ++partition_iter, ++cached_partition_iter) {
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
          // Check and skip ages we don't care about.
          if ((*category_iter)->min_age_ + data_offset < min_age_)
            continue;
          if ((*category_iter)->min_age_ + data_offset > max_age_ && !age_plus_)
            break;

          unsigned age_offset = ( (*category_iter)->min_age_ + data_offset) - min_age_;
          unsigned age        = ( (*category_iter)->min_age_ + data_offset);
          if (min_age_ + age_offset > max_age_)
            age_offset = age_spread_ - 1;

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

      if (expected_values.size() != proportions_[category_offset].size())
        LOG_CODE_ERROR("expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size(" << proportions_[category_offset].size() << ")");

      /**
       * Convert the expected_values in to a proportion
       */
      for (Double& value : expected_values)
        value = value / running_total;

      /**
       * save our comparisons so we can use them to generate the score from the likelihoods later
       */
      for (unsigned i = 0; i < expected_values.size(); ++i) {
        SaveComparison(category_labels_[category_offset], min_age_ + i, expected_values[i], proportions_[category_offset][i],
            process_error_, error_values_by_category_[category_offset][i], delta_, 0.0);
      }
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsAtAge::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    scores_[year_] = likelihood_->GetInitialScore(comparisons_);
    likelihood_->GetScores(comparisons_);

    for (obs::Comparison comparison : comparisons_[year_]) {
      scores_[year_] += comparison.score_;
    }
  }
}

} /* namespace observations */
} /* namespace isam */
