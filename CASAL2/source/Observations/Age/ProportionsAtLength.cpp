/**
 * @file ProportionsAtLength.cpp
 * @author  C Marsh
 * @version 1.0
 * @date 12.8.15
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 */

// Headers
#include "ProportionsAtLength.h"

#include <algorithm>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProportionsAtLength::ProportionsAtLength(shared_ptr<Model> model) : Observation(model) {
  obs_table_          = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tolerance_, "The tolerance for rescaling proportions", "", Double(0.001))->set_lower_bound(0.0, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities", "", true);
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "", true);  // optional
  parameters_.Bind<bool>(PARAM_LENGTH_PLUS, &length_plus_, "Is the last length bin a plus group? (defaults to @model value)", "",
                         model->length_plus());  // default to the model value
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note that the units depend on the likelihood)", "", false);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
}

/**
 * Destructor
 */
ProportionsAtLength::~ProportionsAtLength() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtLength::DoValidate() {
  // How many elements are expected in our observed table;
  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }

  map<unsigned, vector<double>> error_values_by_year;
  map<unsigned, vector<double>> obs_by_year;

  if (process_error_values_.size() != 0 && process_error_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provided (" << process_error_values_.size() << ") does not match the number of years provided (" << years_.size()
                                      << ")";
  }

  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
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

  vector<double> model_length_bins = model_->length_bins();
  if (length_bins_.size() == 0) {
    length_bins_ = model_length_bins;
    // length_plus_     = model_->length_plus();
    mlb_index_first_ = 0;
  } else {
    // allow for the use of observation-defined length bins, as long as all values are in the set of model length bin values
    for (unsigned length = 0; length < length_bins_.size(); ++length) {
      if (length_bins_[length] < 0.0)
        LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bin values must be positive. '" << length_bins_[length] << "' is less than 0";

      if (length > 0 && length_bins_[length - 1] >= length_bins_[length])
        LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bins must be strictly increasing. " << length_bins_[length - 1] << " is greater than or equal to "
                                       << length_bins_[length];

      if (std::find(model_length_bins.begin(), model_length_bins.end(), length_bins_[length]) == model_length_bins.end())
        LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bin values must be in the set of model length bins. Length '" << length_bins_[length]
                                       << "' is not in the set of model length bins.";
    }

    // check that the observation length bins exactly match a sequential subset of the model length bins
    auto it_first = std::find(model_length_bins.begin(), model_length_bins.end(), length_bins_[0]);
    auto it_last  = std::find(model_length_bins.begin(), model_length_bins.end(), length_bins_[(length_bins_.size() - 1)]);
    if (((unsigned)(abs(std::distance(it_first, it_last))) + 1) != length_bins_.size()) {
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bin values must be a sequential subset of model length bins."
                                     << " Length of subset of model length bin sequence: " << (std::distance(it_first, it_last) + 1)
                                     << ", observation length bins: " << length_bins_.size();
    }

    mlb_index_first_ = labs(std::distance(model_length_bins.begin(), it_first));
    LOG_FINE() << "Index of observation length bin in model length bins: " << mlb_index_first_ << ", length_bins_[0] " << length_bins_[0] << ", model length bin "
               << model_length_bins[mlb_index_first_];
  }

  // model vs. observation consistency length_plus check
  if (!(model_->length_plus()) && length_plus_ && length_bins_.back() == model_length_bins.back())
    LOG_ERROR() << "Mismatch between @model length_plus and observation " << label_ << " length_plus for the last length bin";

  number_bins_                         = length_plus_ ? length_bins_.size() : length_bins_.size() - 1;
  unsigned                obs_expected = (category_labels_.size() * number_bins_) + 1;
  vector<vector<string>>& obs_data     = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    for (auto x : obs_data_line)
      if (obs_data_line.size() != obs_expected) {
        LOG_FATAL_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but " << obs_expected << " should match the number of bins (" << number_bins_
                               << ") * categories (" << category_labels_.size() << ") + 1 (for year)";
      }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a Double";
      if (value < 0.0)
        LOG_ERROR_P(PARAM_OBS) << " proportion value (" << value << ") cannot be less than 0.0";

      obs_by_year[year].push_back(value);
    }

    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_OBS) << " " << obs_by_year[year].size() << " lengths were supplied, but " << obs_expected - 1 << " lengths are required";

    // TODO:  need proportion checks here
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
                                      << " should match the number of bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(error_values_data_line[i], value))
        LOG_ERROR_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a Double";

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

    if (error_values_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " " << error_values_by_year[year].size() << " error values by year but " << obs_expected - 1
                                      << " values are required based on the obs table";
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
      for (unsigned j = 0; j < number_bins_; ++j) {
        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end()) {
          unsigned obs_index = i * number_bins_ + j;
          value              = iter->second[obs_index];
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          proportions_[iter->first][category_labels_[i]].push_back(value);
          total += value;
        }
      }
    }

    if (fabs(1.0 - total) > tolerance_) {
      LOG_ERROR_P(PARAM_OBS) << ": obs sum total (" << total << ") for year (" << iter->first << ") exceeds tolerance (" << tolerance_ << ") from 1.0";
    }

    // TODO: rescale proportions if necessary
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void ProportionsAtLength::DoBuild() {
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

  // Build Selectivity pointers
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

  length_results_.resize(number_bins_ * category_labels_.size(), 0.0);
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void ProportionsAtLength::PreExecute() {
  cached_partition_->BuildCache();

  if (cached_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 * Execute the ProportionsAtLength expected values calculations
 */
void ProportionsAtLength::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter = cached_partition_->Begin();
  auto partition_iter        = partition_->Begin();  // vector<vector<partition::Category> >

  vector<Double> expected_values(number_bins_, 0.0);

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each length using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    LOG_FINEST() << "category: " << category_labels_[category_offset];
    Double start_value = 0.0;
    Double end_value   = 0.0;
    Double final_value = 0.0;

    std::fill(expected_values.begin(), expected_values.end(), 0.0);

    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */
    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      LOG_FINEST() << "Selectivity for " << category_labels_[category_offset] << " selectivity " << selectivities_[category_offset]->label();

      (*category_iter)->PopulateAgeLengthMatrix(selectivities_[category_offset]);
      (*category_iter)->CollapseAgeLengthDataToLength();

      (*cached_category_iter)->PopulateCachedAgeLengthMatrix(selectivities_[category_offset]);
      (*cached_category_iter)->CollapseCachedAgeLengthDataToLength();

      for (unsigned length_offset = 0; length_offset < number_bins_; ++length_offset) {
        // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
        start_value = (*cached_category_iter)->cached_length_data_[mlb_index_first_ + length_offset];
        end_value   = (*category_iter)->length_data_[mlb_index_first_ + length_offset];

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else
          final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;

        expected_values[length_offset] += final_value;

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length " << length_bins_[length_offset];
        LOG_FINE() << "Selectivity: " << selectivities_[category_offset]->label();
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
        LOG_FINE() << "expected_value becomes: " << expected_values[length_offset];
      }

      // add the values for the larger lengths to expected_values[(number_bins_ - 1)]
      unsigned last_obs_bin = mlb_index_first_ + number_bins_ - 1;
      if (length_plus_ && (*cached_category_iter)->cached_length_data_.size() > last_obs_bin && (*category_iter)->length_data_.size() > last_obs_bin) {
        for (unsigned length_idx = (last_obs_bin + 1); length_idx < (*cached_category_iter)->cached_length_data_.size(); ++length_idx) {
          start_value = (*cached_category_iter)->cached_length_data_[length_idx];
          end_value   = (*category_iter)->length_data_[length_idx];

          if (mean_proportion_method_)
            final_value = start_value + ((end_value - start_value) * proportion_of_time_);
          else
            final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;

          expected_values[(number_bins_ - 1)] += final_value;

          LOG_FINE() << "----------";
          LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length (length_plus) " << length_bins_[(number_bins_ - 1)];
          LOG_FINE() << "Selectivity: " << selectivities_[category_offset]->label();
          LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
          LOG_FINE() << "expected_value (length_plus) becomes: " << expected_values[(number_bins_ - 1)];
        }
      }
    }

    if (expected_values.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size("
                       << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values.size(); ++i) {
      SaveComparison(category_labels_[category_offset], 0, length_bins_[i], expected_values[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsAtLength::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
    for (auto& iter : comparisons_) {
      double total = 0.0;
      for (auto& comparison : iter.second) total += comparison.observed_;
      for (auto& comparison : iter.second) comparison.observed_ /= total;
    }
  } else {
    /**
     * Convert the expected_values in to a proportion
     */
    for (unsigned year : years_) {
      Double running_total = 0.0;
      for (obs::Comparison comparison : comparisons_[year]) {
        running_total += comparison.expected_;
      }
      for (obs::Comparison& comparison : comparisons_[year]) {
        if (running_total != 0.0)
          comparison.expected_ = comparison.expected_ / running_total;
        else
          comparison.expected_ = 0.0;
      }
    }

    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation neglogLikelihood calculation";
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood: " << scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }

    LOG_FINEST() << "Finished calculating neglogLikelihood for = " << label_;
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
