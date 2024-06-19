/**
 * @file ProportionsByCategory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "ProportionsByCategory.h"

#include <algorithm>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProportionsByCategory::ProportionsByCategory(shared_ptr<Model> model) : Observation(model) {
  obs_table_          = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The maximum age", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Use the age plus group?", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities", "", true);
  parameters_.Bind<string>(PARAM_TARGET_CATEGORIES, &target_category_labels_, "The target categories", "");
  parameters_.Bind<string>(PARAM_TARGET_SELECTIVITIES, &target_selectivity_labels_, "The target selectivities", "");
  // TODO:  is tolerance missing?
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note that the units depend on the likelihood)", "", false);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
}

/**
 * Destructor
 */
ProportionsByCategory::~ProportionsByCategory() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProportionsByCategory::DoValidate() {
  unsigned expected_selectivity_count = 0;
  auto     categories                 = model_->categories();
  for (const string& category_label : category_labels_) expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  if (category_labels_.size() != target_category_labels_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << ": Number of categories(" << category_labels_.size() << ") does not match the number of " PARAM_TARGET_CATEGORIES << "("
                                  << target_category_labels_.size() << ")";

  if (target_category_labels_.size() != target_selectivity_labels_.size() && expected_selectivity_count != target_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_TARGET_SELECTIVITIES) << ": Number of selectivities provided (" << target_selectivity_labels_.size()
                                            << ") is not valid. Specify either the number of category collections (" << target_category_labels_.size() << ") or "
                                            << "the number of total categories (" << expected_selectivity_count << ")";

  age_spread_ = (max_age_ - min_age_) + 1;

  map<unsigned, vector<double>> error_values_by_year;
  map<unsigned, vector<double>> obs_by_year;

  /**
   * Do some simple checks
   */
  if (min_age_ < model_->min_age())
    LOG_ERROR_P(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model_->min_age() << ")";

  if (max_age_ > model_->max_age())
    LOG_ERROR_P(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model_->max_age() << ")";

  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }

  // if only one value supplied then assume its the same for all years
  if (process_error_values_.size() == 1) {
    Double temp = process_error_values_[0];
    process_error_values_.resize(years_.size(), temp);
  }

  if (process_error_values_.size() != 0) {
    if (process_error_values_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_PROCESS_ERRORS) << "Supply a process error for each year. Values for " << process_error_values_.size() << " years were provided, but " << years_.size()
                                        << " years are required";
    }
    process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);
  } else {
    Double process_val      = 0.0;
    process_errors_by_year_ = utilities::Map::create(years_, process_val);
  }

  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";

  if (category_labels_.size() != selectivity_labels_.size() && expected_selectivity_count_ != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Number of selectivities provided (" << selectivity_labels_.size()
                                     << ") is not valid. Specify either the number of category collections (" << category_labels_.size() << ") or "
                                     << "the number of total categories (" << expected_selectivity_count_ << ")";
  /**
   * Validate the number of obs provided matches age spread * category_labels * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned                obs_expected = age_spread_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data     = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << "has " << obs_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    unsigned year = 0;

    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << "has " << obs_data_line.size() << " values defined, but " << obs_expected << " should match the age spread * categories + 1 (for year)";
      return;
    }

    if (!utilities::To<unsigned>(obs_data_line[0], year)) {
      LOG_ERROR_P(PARAM_OBS) << "value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
      return;
    }

    if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_OBS) << "value " << year << " is not a valid year for this observation";
      return;
    }

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << "value (" << obs_data_line[i] << ") could not be converted to a Double";
      // TODO:  need additional proportion checks
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << obs_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_ERROR_VALUES) << "has " << error_values_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    unsigned year = 0;

    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << "has " << error_values_data_line.size() << " values defined, but " << obs_expected
                                      << " should match the age spread * categories + 1 (for year)";
    } else if (!utilities::To<unsigned>(error_values_data_line[0], year)) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << "value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    } else if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << "value " << year << " is not a valid year for this observation";
    } else {
      for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
        double value = 0.0;
        if (!utilities::To<double>(error_values_data_line[i], value))
          LOG_ERROR_P(PARAM_ERROR_VALUES) << "value (" << error_values_data_line[i] << ") could not be converted to a Double";
        if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
          LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be equal to or less than 0.0";
        } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
          LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
        }

        error_values_by_year[year].push_back(value);
      }

      if (error_values_by_year[year].size() == 1) {
        auto val_e = error_values_by_year[year][0];
        error_values_by_year[year].assign(obs_expected - 1, val_e);
      }

      if (error_values_by_year[year].size() != obs_expected - 1) {
        LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << error_values_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
      }
    }
  }

  /**
   * Build our proportions and error values for use in the observation
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;

        if (!utilities::To<double>(iter->second[obs_index], value))
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1 << " in the definition could not be converted to a Double";

        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end()) {
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          proportions_[iter->first][category_labels_[i]].push_back(value);
          total += value;
        }
      }
    }

    // if (!utilities::math::IsOne(total)) {
    //   LOG_WARNING()  << "obs sum total (" << total << ") for year (" << iter->first << ") doesn't sum to 1.0";
    // }
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void ProportionsByCategory::DoBuild() {
  partition_               = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_        = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  target_partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, target_category_labels_));
  target_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, target_category_labels_));

  if (ageing_error_label_ != "")
    LOG_CODE_ERROR() << "ageing error has not been implemented for proportions at age observations";

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  for (string label : target_selectivity_labels_) {
    auto selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_TARGET_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    } else
      target_selectivities_.push_back(selectivity);
  }

  if (target_selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_t = target_selectivities_[0];
    target_selectivities_.assign(category_labels_.size(), val_t);
  }
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void ProportionsByCategory::PreExecute() {
  cached_partition_->BuildCache();
  target_cached_partition_->BuildCache();

  if (cached_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 * Execute
 */
void ProportionsByCategory::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter        = cached_partition_->Begin();
  auto partition_iter               = partition_->Begin();  // vector<vector<partition::Category> >
  auto target_cached_partition_iter = target_cached_partition_->Begin();
  auto target_partition_iter        = target_partition_->Begin();  // vector<vector<partition::Category> >

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    Double selectivity_result = 0.0;
    Double start_value        = 0.0;
    Double end_value          = 0.0;
    Double final_value        = 0.0;

    vector<Double> age_results(age_spread_, 0.0);
    vector<Double> target_age_results(age_spread_, 0.0);

    /**
     * Loop through the 2 combined categories building up the
     * age results proportions values.
     */
    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        // Check and skip ages we don't care about.
        if ((*category_iter)->min_age_ + data_offset < min_age_)
          continue;
        if ((*category_iter)->min_age_ + data_offset > max_age_ && !plus_group_)
          break;

        unsigned age_offset = ((*category_iter)->min_age_ + data_offset) - min_age_;
        unsigned age        = ((*category_iter)->min_age_ + data_offset);

        if (age < min_age_)
          continue;
        if (age > max_age_)
          break;

        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;

        LOG_FINE() << "---------------";
        LOG_FINE() << "age: " << age;
        selectivity_result = selectivities_[category_offset]->GetAgeResult(age, (*category_iter)->age_length_);
        start_value        = (*cached_category_iter)->cached_data_[data_offset];
        end_value          = (*category_iter)->data_[data_offset];
        final_value        = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else
          final_value = fabs(start_value - end_value) * proportion_of_time_;

        LOG_FINE() << "Category1:" << (*category_iter)->name_;
        LOG_FINE() << "selectivity_result: " << selectivity_result;
        LOG_FINE() << "start_value: " << start_value << " / end_value: " << end_value;
        LOG_FINE() << "final_value: " << final_value;
        LOG_FINE() << "final_value * selectivity: " << (Double)(final_value * selectivity_result);

        age_results[age_offset] += final_value * selectivity_result;
        LOG_FINE() << "category1 becomes: " << age_results[age_offset];
      }
    }

    /**
     * Loop through the target combined categories building up the
     * target age results
     */
    auto target_category_iter        = target_partition_iter->begin();
    auto target_cached_category_iter = target_cached_partition_iter->begin();
    for (; target_category_iter != target_partition_iter->end(); ++target_cached_category_iter, ++target_category_iter) {
      for (unsigned data_offset = 0; data_offset < (*target_category_iter)->data_.size(); ++data_offset) {
        // Check and skip ages we don't care about.
        if ((*target_category_iter)->min_age_ + data_offset < min_age_)
          continue;
        if ((*target_category_iter)->min_age_ + data_offset > max_age_ && !plus_group_)
          break;

        unsigned age_offset = ((*target_category_iter)->min_age_ + data_offset) - min_age_;
        unsigned age        = ((*target_category_iter)->min_age_ + data_offset);
        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;
        if (age < min_age_)
          continue;
        if (age > max_age_)
          break;

        selectivity_result = target_selectivities_[category_offset]->GetAgeResult(age, (*target_category_iter)->age_length_);
        start_value        = (*target_cached_category_iter)->cached_data_[data_offset];
        end_value          = (*target_category_iter)->data_[data_offset];
        final_value        = 0.0;

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

    if (age_results.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << age_results.size() << ") != proportions_[category_offset].size("
                       << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < age_results.size(); ++i) {
      Double expected = 0.0;
      if (age_results[i] != 0.0)
        expected = target_age_results[i] / age_results[i];

      SaveComparison(category_labels_[category_offset], min_age_ + i, 0, expected, proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsByCategory::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

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
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
