/**
 * @file TagRecaptureByAge.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 23/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "TagRecaptureByAge.h"

#include <algorithm>

#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
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
TagRecaptureByAge::TagRecaptureByAge(shared_ptr<Model> model) : Observation(model) {
  recaptures_table_ = new parameters::Table(PARAM_RECAPTURED);
  scanned_table_    = new parameters::Table(PARAM_SCANNED);
  // clang-format off
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The maximum age", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Is the maximum age the age plus group?", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities of the scanned categories", "", true);
  parameters_.Bind<string>(PARAM_TAGGED_CATEGORIES, &tagged_category_labels_, "The categories of tagged individuals", "");
  parameters_.Bind<string>(PARAM_TAGGED_SELECTIVITIES, &tagged_selectivity_labels_, "The labels of the selectivities of the tagged categories ", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<Double>(PARAM_DETECTION_PARAMETER, &detection_, "The probability of detecting a recaptured individual", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_DISPERSION, &dispersion_, "The overdispersion parameter (phi)  ", "", true)->set_lower_bound(0.0);
  parameters_.BindTable(PARAM_RECAPTURED, recaptures_table_, "The table of observed recaptured individuals in each age class", "", false);
  parameters_.BindTable(PARAM_SCANNED, scanned_table_, "The table of observed scanned individuals in each age class", "", false);
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "", Double(0.5))->set_range(0.0, 1.0);
  // clang-format on

  RegisterAsAddressable(PARAM_DETECTION_PARAMETER, &detection_);

  mean_proportion_method_ = true;

  allowed_likelihood_types_.push_back(PARAM_BINOMIAL);
}

/**
 * Validate configuration file parameters
 */
void TagRecaptureByAge::DoValidate() {
  unsigned expected_selectivity_count = 0;
  auto     categories                 = model_->categories();
  for (const string& category_label : category_labels_) expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ")";
  }

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << ": Number of categories (" << category_labels_.size() << ") does not match the number of " PARAM_SELECTIVITIES << " ("
                                  << selectivity_labels_.size() << ")";

  if (tagged_category_labels_.size() != tagged_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": Number of tagged categories (" << tagged_category_labels_.size() << ") does not match the number of " PARAM_TAGGED_SELECTIVITIES
                                         << " (" << tagged_selectivity_labels_.size() << ")";

  age_spread_ = (max_age_ - min_age_) + 1;
  map<unsigned, vector<double>> recaptures_by_year;
  map<unsigned, vector<double>> scanned_by_year;

  // Do some simple checks
  if (min_age_ < model_->min_age())
    LOG_ERROR_P(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model_->min_age() << ")";
  if (max_age_ > model_->max_age())
    LOG_ERROR_P(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model_->max_age() << ")";
  if (detection_ < 0.0 || detection_ > 1.0)
    LOG_ERROR_P(PARAM_DETECTION_PARAMETER) << ": detection probability must be between 0.0 and 1.0 (inclusive)";
  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";

  // Validate the number of recaptures provided matches age spread* category_labels* years
  //  This is because we'll have 1 set of recaptures per category collection provided.
  //  categories male+female male = 2 collections
  unsigned                obs_expected    = age_spread_ * category_labels_.size() + 1;
  vector<vector<string>>& recaptures_data = recaptures_table_->data();

  if (recaptures_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_RECAPTURED) << "has " << recaptures_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& recaptures_data_line : recaptures_data) {
    unsigned year = 0;

    if (recaptures_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_RECAPTURED) << "has " << recaptures_data_line.size() << " values defined, but " << obs_expected
                                    << " should match the age spread * categories + 1 (for year)";
      return;
    }

    if (!utilities::To<unsigned>(recaptures_data_line[0], year)) {
      LOG_ERROR_P(PARAM_RECAPTURED) << "value " << recaptures_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
      return;
    }

    if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_RECAPTURED) << "value " << year << " is not a valid year for this observation";
      return;
    }

    for (unsigned i = 1; i < recaptures_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(recaptures_data_line[i], value))
        LOG_ERROR_P(PARAM_RECAPTURED) << "value (" << recaptures_data_line[i] << ") could not be converted to a Double";
      recaptures_by_year[year].push_back(value);
    }
    if (recaptures_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << recaptures_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
  }

  // Build our scanned map
  vector<vector<string>>& scanned_values_data = scanned_table_->data();
  if (scanned_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_SCANNED) << "has " << scanned_values_data.size() << " rows defined, but" << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& scanned_values_data_line : scanned_values_data) {
    unsigned year = 0;

    if (scanned_values_data_line.size() != 2 && scanned_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_SCANNED) << "has " << scanned_values_data_line.size() << " values defined, but " << obs_expected
                                 << " should match the age spread * categories + 1 (for year)";
    } else if (!utilities::To<unsigned>(scanned_values_data_line[0], year)) {
      LOG_ERROR_P(PARAM_SCANNED) << "value " << scanned_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    } else if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_SCANNED) << "value " << year << " is not a valid year for this observation";
    } else {
      for (unsigned i = 1; i < scanned_values_data_line.size(); ++i) {
        double value = 0.0;
        if (!utilities::To<double>(scanned_values_data_line[i], value)) {
          LOG_ERROR_P(PARAM_SCANNED) << "value (" << scanned_values_data_line[i] << ") could not be converted to a Double";
        } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
          LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
        }

        scanned_by_year[year].push_back(value);
      }
      if (scanned_by_year[year].size() == 1) {
        auto val_s = scanned_by_year[year][0];
        scanned_by_year[year].assign(obs_expected - 1, val_s);
      }
      if (scanned_by_year[year].size() != obs_expected - 1) {
        LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << scanned_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
      }
    }
  }

  /**
   * Build our Recaptured and scanned maps for use in the DoExecute() section
   */
  double value = 0.0;
  for (auto iter = recaptures_by_year.begin(); iter != recaptures_by_year.end(); ++iter) {
    double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;
        if (!utilities::To<double>(iter->second[obs_index], value)) {
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1 << " in the definition could not be converted to a Double";
        }

        auto s_f = scanned_by_year.find(iter->first);
        if (s_f != scanned_by_year.end()) {
          auto error_value = s_f->second[obs_index];
          scanned_[iter->first][category_labels_[i]].push_back(error_value);
          recaptures_[iter->first][category_labels_[i]].push_back(value);
          total += error_value;
        }
      }
    }
  }

  // validate and build dispersion (e.g. process_error)
  for (Double dispersion : dispersion_) {
    if (dispersion < 0.0)
      LOG_ERROR_P(PARAM_DISPERSION) << ": dispersion (" << AS_DOUBLE(dispersion) << ") cannot be less than 0.0";
  }
  // if only one value supplied then assume its the same for all years
  if (dispersion_.size() == 1) {
    dispersion_.resize(years_.size(), dispersion_[0]);
  }

  if (dispersion_.size() != 0) {
    if (dispersion_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_DISPERSION) << "Supply one value of dispersion that is used for every year, or a value of dispersion for each year. Values for " << dispersion_.size()
                                    << " years were supplied, but " << years_.size() << " years are required";
    }
    dispersion_by_year_ = utilities::Map::create(years_, dispersion_);
  } else {
    Double dispersion_val = 1.0;
    dispersion_by_year_   = utilities::Map::create(years_, dispersion_val);
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void TagRecaptureByAge::DoBuild() {
  partition_               = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_        = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  tagged_partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, tagged_category_labels_));
  tagged_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, tagged_category_labels_));

  if (ageing_error_label_ != "") {
    LOG_CODE_ERROR() << "ageing error has not been implemented for the tag recapture at age observation";
  }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  // Build Selectivity pointers
  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " does not exist.";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  for (string label : tagged_selectivity_labels_) {
    auto selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_TAGGED_SELECTIVITIES) << ": Selectivity label " << label << " does not exist.";
    } else
      tagged_selectivities_.push_back(selectivity);
  }

  if (tagged_selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_t = tagged_selectivities_[0];
    tagged_selectivities_.assign(category_labels_.size(), val_t);
  }

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0 inclusive";
  proportion_of_time_ = time_step_proportion_;

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) time_step->SubscribeToBlock(this, year);
  }
}

/**
 * This method is called at the start of the targeted time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void TagRecaptureByAge::PreExecute() {
  cached_partition_->BuildCache();
  tagged_cached_partition_->BuildCache();

  if (cached_partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "cached_partition_->Size() != scanned_[model->current_year()].size()";
  }
  if (partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "partition_->Size() != scanned_[model->current_year()].size()";
  }
}

/**
 * Execute
 */
void TagRecaptureByAge::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter        = cached_partition_->Begin();
  auto partition_iter               = partition_->Begin();  // vector<vector<partition::Category> >
  auto tagged_cached_partition_iter = tagged_cached_partition_->Begin();
  auto tagged_partition_iter        = tagged_partition_->Begin();  // vector<vector<partition::Category> >

  vector<Double> age_results(age_spread_);
  vector<Double> tagged_age_results(age_spread_);

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

    std::fill(age_results.begin(), age_results.end(), 0.0);
    std::fill(tagged_age_results.begin(), tagged_age_results.end(), 0.0);

    /**
     * Loop through the 2 combined categories if they are supplied, building up the
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

        // Numbers at age from the population
        age_results[age_offset] += final_value * selectivity_result;
        LOG_FINE() << "category1 becomes: " << age_results[age_offset];
      }
    }

    /**
     * Loop through the tagged combined categories building up the
     * tagged age results
     */
    auto tagged_category_iter        = tagged_partition_iter->begin();
    auto tagged_cached_category_iter = tagged_cached_partition_iter->begin();
    for (; tagged_category_iter != tagged_partition_iter->end(); ++tagged_cached_category_iter, ++tagged_category_iter) {
      for (unsigned data_offset = 0; data_offset < (*tagged_category_iter)->data_.size(); ++data_offset) {
        // Check and skip ages we don't care about.
        if ((*tagged_category_iter)->min_age_ + data_offset < min_age_)
          continue;
        if ((*tagged_category_iter)->min_age_ + data_offset > max_age_ && !plus_group_)
          break;

        unsigned age_offset = ((*tagged_category_iter)->min_age_ + data_offset) - min_age_;
        unsigned age        = ((*tagged_category_iter)->min_age_ + data_offset);
        if (min_age_ + age_offset > max_age_)
          age_offset = age_spread_ - 1;
        if (age < min_age_)
          continue;
        if (age > max_age_)
          break;

        selectivity_result = tagged_selectivities_[category_offset]->GetAgeResult(age, (*tagged_category_iter)->age_length_);
        start_value        = (*tagged_cached_category_iter)->cached_data_[data_offset];
        end_value          = (*tagged_category_iter)->data_[data_offset];
        final_value        = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else
          final_value = fabs(start_value - end_value) * proportion_of_time_;

        LOG_FINE() << "----------";
        LOG_FINE() << "Category2:" << (*tagged_category_iter)->name_;
        LOG_FINE() << "age: " << age;
        LOG_FINE() << "selectivity_result: " << selectivity_result;
        LOG_FINE() << "start_value: " << start_value << " / end_value: " << end_value;
        LOG_FINE() << "final_value: " << final_value;
        LOG_FINE() << "final_value * selectivity: " << (Double)(final_value * selectivity_result);
        tagged_age_results[age_offset] += final_value * selectivity_result;
        LOG_FINE() << "category2 becomes: " << tagged_age_results[age_offset];
      }
    }

    if (age_results.size() != scanned_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << age_results.size() << ") != scanned_[category_offset].size("
                       << scanned_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    // save our comparisons so we can use them to generate the score from the likelihoods later
    for (unsigned i = 0; i < age_results.size(); ++i) {
      Double expected = 0.0;
      double observed = 0.0;
      if (age_results[i] != 0.0)
        expected = detection_ * tagged_age_results[i] / age_results[i];
      if (scanned_[model_->current_year()][category_labels_[category_offset]][i] == 0.0)
        observed = 0.0;
      else
        observed = (recaptures_[model_->current_year()][category_labels_[category_offset]][i]) / scanned_[model_->current_year()][category_labels_[category_offset]][i];

      LOG_MEDIUM() << "Comparison for age " << min_age_ + i << ": expected = " << expected << " observed = " << observed
                   << " error = " << scanned_[model_->current_year()][category_labels_[category_offset]][i]
                   << " recaptures = " << recaptures_[model_->current_year()][category_labels_[category_offset]][i];

      // process_error is not used here, and the dispersion is applied to the final likelihood value below
      SaveComparison(tagged_category_labels_[category_offset], min_age_ + i, 0, expected, observed, 0.0, scanned_[model_->current_year()][category_labels_[category_offset]][i],
                     0.0, delta_, 0.0);
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
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation neglogLikelihood calculation " << label_;
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:" << scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
      // Add the dispersion factor to the likelihood score
      scores_[year] /= dispersion_by_year_[year];
    }

    LOG_FINEST() << "Finished calculating neglogLikelihood for = " << label_;
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
