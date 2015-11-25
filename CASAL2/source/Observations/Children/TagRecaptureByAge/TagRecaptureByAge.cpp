/**
 * @file TagRecaptureByAge.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 23/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "TagRecaptureByAge.h"

#include <algorithm>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace observations {

/**
 * Default constructor
 */
TagRecaptureByAge::TagRecaptureByAge(Model* model) : Observation(model) {
  recaptures_table_ = new parameters::Table(PARAM_RECAPTURED);
  scanned_table_ = new parameters::Table(PARAM_SCANNED);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Use age plus group", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Year to execute in", "");
  parameters_.Bind<string>(PARAM_TARGET_CATEGORIES, &target_category_labels_, "Target Categories", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "Selectivity labels to use", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute in", "");
  parameters_.Bind<string>(PARAM_TARGET_SELECTIVITIES, &target_selectivity_labels_, "Target Selectivities", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta", "", DELTA)->set_lower_bound(0.0, false);
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "Process error", "", true);
  parameters_.Bind<Double>(PARAM_DETECTION_PARAMETER,  &detection_, "Detection probability ", "");
  parameters_.BindTable(PARAM_RECAPTURED, recaptures_table_, "Table of Recaptures", "", false);
  parameters_.BindTable(PARAM_SCANNED, scanned_table_, "Table of scanned individuals", "", false);
}

/**
 * Validate configuration file parameters
 */
void TagRecaptureByAge::DoValidate() {

  unsigned expected_selectivity_count = 0;
  auto categories = model_->categories();
  for (const string& category_label : category_labels_)
    expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << ": Number of categories(" << category_labels_.size() << ") does not match the number of "
    PARAM_SELECTIVITIES << "(" << selectivity_labels_.size() << ")";
  if (target_category_labels_.size() != target_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_TARGET_CATEGORIES) << ": Number of selectivities provided (" << target_selectivity_labels_.size()
    << ") is not valid. You can specify either the number of category collections (" << target_category_labels_.size() << ") or "
    << "the number of total categories (" << expected_selectivity_count << ")";

  age_spread_ = (max_age_ - min_age_) + 1;
  map<unsigned, vector<Double>> recaptures_by_year;
  map<unsigned, vector<Double>> scanned_by_year;

  /**
   * Do some simple checks
   */
  if (min_age_ < model_->min_age())
    LOG_ERROR_P(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model_->min_age() << ")";
  if (max_age_ > model_->max_age())
    LOG_ERROR_P(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model_->max_age() << ")";
  if (detection_ < 0.0 || detection_ > 1.0)
    LOG_ERROR_P(PARAM_DETECTION_PARAMETER) << ": detection probability must be between 0 and 1";


  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << AS_DOUBLE(delta_) << ") cannot be less than 0.0";

  /**
   * Validate the number of recaptures provided matches age spread * category_labels * years
   * This is because we'll have 1 set of recaptures per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned obs_expected = age_spread_ * category_labels_.size() + 1;
  vector<vector<string>>& recpatures_data = recaptures_table_->data();
  if (recpatures_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recpatures_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& recaptures_data_line : recpatures_data) {
    unsigned year = 0;

    if (recaptures_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recaptures_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age_spread * categories + 1 (for year)";
      return;
    }

    if (!utilities::To<unsigned>(recaptures_data_line[0], year)) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << recaptures_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
      return;
    }

    if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << year << " is not a valid year for this observation";
      return;
    }

    for (unsigned i = 1; i < recaptures_data_line.size(); ++i) {
      Double value = 0;
      if (!utilities::To<Double>(recaptures_data_line[i], value))
        LOG_ERROR_P(PARAM_RECAPTURED) << " value (" << recaptures_data_line[i] << ") could not be converted to a double";
      recaptures_by_year[year].push_back(value);
    }
    if (recaptures_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << recaptures_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")";

  }

  /**
   * Build our scanned map
   */
  vector<vector<string>>& scanned_values_data = scanned_table_->data();
  if (scanned_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& scanned_values_data_line : scanned_values_data) {
    unsigned year = 0;

    if (scanned_values_data_line.size() != 2 && scanned_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age speard * categories + 1 (for year)";
    } else if (!utilities::To<unsigned>(scanned_values_data_line[0], year)) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << scanned_values_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    } else if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << year << " is not a valid year for this observation";
    } else {
        for (unsigned i = 1; i < scanned_values_data_line.size(); ++i) {
          Double value = 0;
        if (!utilities::To<Double>(scanned_values_data_line[i], value)) {
          LOG_ERROR_P(PARAM_SCANNED) << " value (" << scanned_values_data_line[i] << ") could not be converted to a double";
        } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
            LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << AS_DOUBLE(value) << ") cannot be less than 0.0";
        }

        scanned_by_year[year].push_back(value);
      }
      if (scanned_by_year[year].size() == 1) {
        scanned_by_year[year].assign(obs_expected - 1, scanned_by_year[year][0]);
      }
      if (scanned_by_year[year].size() != obs_expected - 1) {
        LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << scanned_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")";
      }
    }
  }

  /**
   * Validate likelihood type
   */
  if (likelihood_type_ != PARAM_BINOMIAL) {
    LOG_ERROR_P(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the proportions at age observation. "
        << "Supported types are " << PARAM_BINOMIAL;
  }

  /**
   * Build our Recaptured and scanned maps for use in the DoExecute() section
   */
  Double value = 0.0;
  for (auto iter = recaptures_by_year.begin(); iter != recaptures_by_year.end(); ++iter) {
    Double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;
        if (!utilities::To<Double>(iter->second[obs_index], value)) {
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1
              << " in the definition could not be converted to a numeric double";
        }

        Double error_value = scanned_by_year[iter->first][obs_index];
        scanned_[iter->first][category_labels_[i]].push_back(error_value);
        recaptures_[iter->first][category_labels_[i]].push_back(value);
        total += error_value;
      }
    }

//    if (fabs(1.0 - total) > tolerance_) {
//      LOG_ERROR_P(PARAM_OBS) << ": obs sum total (" << total << ") for year (" << iter->first << ") exceeds tolerance (" << tolerance_ << ") from 1.0";
//    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void TagRecaptureByAge::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  target_partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, target_category_labels_));
  target_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, target_category_labels_));


  if (ageing_error_label_ != "") {
    LOG_CODE_ERROR() << "ageing error has not been implemented for the proportions at age observation";
  }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  // Build Selectivity pointers
  for(string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist. Have you defined it?";
    selectivities_.push_back(selectivity);
  }
  if (selectivities_.size() == 1 && category_labels_.size() != 1)
    selectivities_.assign(category_labels_.size(), selectivities_[0]);


  for(string label : target_selectivity_labels_) {
    auto selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_TARGET_SELECTIVITIES) << ": " << label << " does not exist. Have you defined it?";
    } else
        target_selectivities_.push_back(selectivity);
  }

  if (target_selectivities_.size() == 1 && category_labels_.size() != 1)
    target_selectivities_.assign(category_labels_.size(), target_selectivities_[0]);
}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void TagRecaptureByAge::PreExecute() {
  cached_partition_->BuildCache();
  target_cached_partition_->BuildCache();

  if (cached_partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  }
  if (partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
  }
}

/**
 *
 */
void TagRecaptureByAge::Execute() {
  LOG_TRACE();

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // vector<vector<partition::Category> >
  auto target_cached_partition_iter  = target_cached_partition_->Begin();
  auto target_partition_iter         = target_partition_->Begin(); // vector<vector<partition::Category> >

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    Double      selectivity_result = 0.0;
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;
    Double      final_value        = 0.0;

    vector<Double> age_results(age_spread_, 0.0);
    vector<Double> target_age_results(age_spread_, 0.0);

    /**
     * Loop through the 2 combined categories if they are supplied, building up the
     * age results proportions values.
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

        if (age < min_age_)
          continue;
        if (age > max_age_)
          break;

        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;

        LOG_FINE() << "---------------";
        LOG_FINE() << "age: " << age;
        selectivity_result = selectivities_[category_offset]->GetResult(age, (*category_iter)->age_length_);
        start_value   = (*cached_category_iter).data_[data_offset];
        end_value     = (*category_iter)->data_[data_offset];
        final_value   = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else
          final_value = fabs(start_value - end_value) * proportion_of_time_;

        LOG_FINE() << "Category1:" << (*category_iter)->name_;
        LOG_FINE() << "selectivity_result: " << selectivity_result;
        LOG_FINE() << "start_value: " << start_value << " / end_value: " << end_value;
        LOG_FINE() << "final_value: " << final_value;
        LOG_FINE() << "final_value * selectivity: " << (Double)(final_value * selectivity_result);

        // Numbers at age from the population
        age_results[age_offset] += final_value * selectivity_result;
        LOG_FINE() << "category1 becomes: " << age_results[age_offset];
      }
    }

    /**
     * Loop through the target combined categories building up the
     * target age results
     */
    auto target_category_iter = target_partition_iter->begin();
    auto target_cached_category_iter = target_cached_partition_iter->begin();
    for (; target_category_iter != target_partition_iter->end(); ++target_cached_category_iter, ++target_category_iter) {
      for (unsigned data_offset = 0; data_offset < (*target_category_iter)->data_.size(); ++data_offset) {
        // Check and skip ages we don't care about.
        if ((*target_category_iter)->min_age_ + data_offset < min_age_)
          continue;
        if ((*target_category_iter)->min_age_ + data_offset > max_age_ && !age_plus_)
          break;

        unsigned age_offset = ( (*target_category_iter)->min_age_ + data_offset) - min_age_;
        unsigned age        = ( (*target_category_iter)->min_age_ + data_offset);
        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;
        if (age < min_age_)
          continue;
        if (age > max_age_)
          break;

        selectivity_result = target_selectivities_[category_offset]->GetResult(age, (*target_category_iter)->age_length_);
        start_value   = (*target_cached_category_iter).data_[data_offset];
        end_value     = (*target_category_iter)->data_[data_offset];
        final_value   = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else
          final_value = fabs(start_value - end_value) * proportion_of_time_;
        LOG_FINE() << "----------";
        LOG_FINE() << "Category2:" << (*target_category_iter)->name_;
        LOG_FINE() << "age: " << age;
        LOG_FINE() << "selectivity_result: " << selectivity_result;
        LOG_FINE() << "start_value: " << start_value << " / end_value: " << end_value;
        LOG_FINE() << "final_value: " << final_value;
        LOG_FINE() << "final_value * selectivity: " << (Double)(final_value * selectivity_result);
        target_age_results[age_offset] += final_value * selectivity_result;
        LOG_FINE() << "category2 becomes: " << target_age_results[age_offset];
      }
    }

    if (age_results.size() != scanned_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << age_results.size() << ") != proportions_[category_offset].size("
        << scanned_[model_->current_year()][category_labels_[category_offset]].size() << ")";


     //save our comparisons so we can use them to generate the score from the likelihoods later


    for (unsigned i = 0; i < age_results.size(); ++i) {
      Double expected = 0.0;
      Double observed = 0.0;
      if (age_results[i] != 0.0)
        expected = target_age_results[i] / age_results[i];
      if (scanned_[model_->current_year()][category_labels_[category_offset]][i] == 0.0)
        observed = 0.0;
      else
        observed = (1 / detection_ * recaptures_[model_->current_year()][category_labels_[category_offset]][i]) / scanned_[model_->current_year()][category_labels_[category_offset]][i];                                                                                                                                                                    ;
      LOG_MEDIUM() << "Comparison for age " << min_age_ + i << " Expected = " << expected << " observed = " << observed << " error = "
          << scanned_[model_->current_year()][category_labels_[category_offset]][i] << " recaptures = " << recaptures_[model_->current_year()][category_labels_[category_offset]][i];
      SaveComparison(target_category_labels_[category_offset], min_age_ + i, 0, expected, observed,
          process_errors_by_year_[model_->current_year()], scanned_[model_->current_year()][category_labels_[category_offset]][i], delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void TagRecaptureByAge::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    /**
     * Convert the expected_values in to a proportion
     */
    for (unsigned year : years_) {
//      Double running_total = 0.0;
//      for (obs::Comparison comparison : comparisons_[year]) {
//        running_total += comparison.expected_;
//      }
//      for (obs::Comparison& comparison : comparisons_[year]) {
//        if (running_total != 0.0)
//          comparison.expected_  = comparison.expected_ / running_total;
//        else
//          comparison.expected_  = 0.0;
//      }

      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      likelihood_->GetScores(comparisons_);
      for (obs::Comparison comparison : comparisons_[year]) {
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace observations */
} /* namespace niwa */
