/**
 * @file ProcessRemovalsByWeight.cpp
 *
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// Headers
#include "ProcessRemovalsByWeight.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <iterator>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "AgeLengths/AgeLength.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// Namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProcessRemovalsByWeight::ProcessRemovalsByWeight(shared_ptr<Model> model) : Observation(model) {
  obs_table_          = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<string>(PARAM_MORTALITY_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation", "");
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "The label of observed method of removals", "", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to execute in", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<Double>(PARAM_LENGTH_WEIGHT_CV, &length_weight_cv_, "The CV for the length-weight relationship", "", double(0.10))->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT_DISTRIBUTION, &length_weight_distribution_label_, "The distribution of the length-weight relationship", "", PARAM_NORMAL)
      ->set_allowed_values({PARAM_NORMAL, PARAM_LOGNORMAL});
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "");
  parameters_.Bind<double>(PARAM_LENGTH_BINS_N, &length_bins_n_, "The average number in each length bin", "");
  parameters_.Bind<string>(PARAM_UNITS, &units_, "The units for the weight bins (grams, kilograms (kgs), or tonnes)", "", PARAM_KGS)
      ->set_allowed_values({PARAM_GRAMS, PARAM_TONNES, PARAM_KGS, PARAM_KILOGRAMS});
  parameters_.Bind<Double>(PARAM_FISHBOX_WEIGHT, &fishbox_weight_, "The target weight of each box", "", double(20.0))->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_WEIGHT_BINS, &weight_bins_, "The weight bins", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note that the units depend on the likelihood)", "", false);

  mean_proportion_method_ = false;

  RegisterAsAddressable(PARAM_PROCESS_ERRORS, &process_error_values_);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);

  allowed_mortality_types_.push_back(PARAM_MORTALITY_INSTANTANEOUS);
}

/**
 * Destructor
 */
ProcessRemovalsByWeight::~ProcessRemovalsByWeight() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProcessRemovalsByWeight::DoValidate() {
  // Check value for initial mortality
  if (model_->length_bins().size() == 0)
    LOG_FATAL_P(PARAM_LENGTH_BINS) << ": No length bins have been specified in @model. This observation requires those to be defined";

  // Need to validate length bins are subclass of mdoel length bins.
  if (!model_->are_length_bin_compatible_with_model_length_bins(length_bins_)) {
    LOG_FATAL_P(PARAM_LENGTH_BINS) << "Length bins need to be a subset of the model length bins. See manual for more information";
  }
  // How many elements are expected in our observed table;
  number_length_bins_ = length_bins_.size();
  number_weight_bins_ = weight_bins_.size();

  if (length_weight_distribution_label_ == PARAM_NORMAL)
    length_weight_distribution_ = Distribution::kNormal;
  else if (length_weight_distribution_label_ == PARAM_LOGNORMAL)
    length_weight_distribution_ = Distribution::kLogNormal;
  else
    LOG_CODE_ERROR() << "The length-weight distribution '" << length_weight_distribution_label_ << "' is not valid.";

  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }

  map<unsigned, vector<double>> error_values_by_year;
  map<unsigned, vector<double>> obs_by_year;

  /**
   * Do some simple checks
   * e.g., validate that the length and weight bins are strictly increasing
   */

  vector<double> model_length_bins = model_->length_bins();
  if (length_bins_[0] < model_length_bins[0])
    LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin value " << length_bins_[0] << " is smaller than the smallest model length bin " << model_length_bins[0];
  if (length_bins_[(number_length_bins_ - 1)] > model_length_bins[(model_length_bins.size() - 1)])
    LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin value " << length_bins_[(number_length_bins_ - 1)] << " is larger than the largest model length bin "
                                   << model_length_bins[(model_length_bins.size() - 1)];

  if (length_bins_.size() != length_bins_n_.size())
    LOG_ERROR_P(PARAM_LENGTH_BINS_N) << ": The number of length bins " << length_bins_.size() << " does not match the number of length bin number values " << length_bins_n_.size();

  for (unsigned length = 0; length < length_bins_.size(); ++length) {
    if (length_bins_[length] < 0.0)
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin values must be positive: " << length_bins_[length] << " is less than 0.0";

    if (length > 0 && length_bins_[length - 1] >= length_bins_[length])
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin values must be strictly increasing: " << length_bins_[length - 1] << " is greater than or equal to " << length_bins_[length];
  }

  for (unsigned length = 0; length < length_bins_n_.size(); ++length) {
    if (length_bins_n_[length] <= 0.0)
      LOG_ERROR_P(PARAM_LENGTH_BINS_N) << ": Length bin number values must be positive: " << length_bins_n_[length] << " is less than or equal to 0.0";
  }

  for (unsigned weight = 0; weight < weight_bins_.size(); ++weight) {
    if (weight_bins_[weight] < 0.0)
      LOG_ERROR_P(PARAM_WEIGHT_BINS) << ": Weight bin values must be positive: " << weight_bins_[weight] << " is less than 0.0";

    if (weight > 0 && weight_bins_[weight - 1] >= weight_bins_[weight])
      LOG_ERROR_P(PARAM_WEIGHT_BINS) << ": Weight bin values must be strictly increasing: " << weight_bins_[weight - 1] << " is greater than or equal to " << weight_bins_[weight];
  }

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

  /**
   * Validate the number of obs provided matches age spread * category_labels * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned                obs_expected = number_weight_bins_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data     = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << "has " << obs_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << "has " << obs_data_line.size() << " values defined, but " << obs_expected << " should match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << "value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << "value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << "value (" << obs_data_line[i] << ") could not be converted to a Double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_OBS) << " " << obs_by_year[year].size() << " weights were provided, but " << obs_expected - 1 << "weights are required";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_FATAL_P(PARAM_ERROR_VALUES) << "has " << error_values_data.size() << " rows defined, but " << years_.size() << " to match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << "has " << error_values_data_line.size() << " values defined, but " << obs_expected
                                      << " to match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;

    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_FATAL_P(PARAM_ERROR_VALUES) << "value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_FATAL_P(PARAM_ERROR_VALUES) << "value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(error_values_data_line[i], value))
        LOG_FATAL_P(PARAM_ERROR_VALUES) << "value (" << error_values_data_line[i] << ") could not be converted to a Double";
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
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " " << error_values_by_year[year].size() << " error values by year were provided, but " << obs_expected - 1
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
      for (unsigned j = 0; j < number_weight_bins_; ++j) {
        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end()) {
          unsigned obs_index = i * number_weight_bins_ + j;
          value              = iter->second[obs_index];
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          proportions_[iter->first][category_labels_[i]].push_back(value);
          total += value;
        }
      }
    }
    if (!utilities::math::IsOne(total)) {
      LOG_WARNING() << "obs sum total (" << total << ") for year (" << iter->first << ") doesn't sum to 1.0";
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProcessRemovalsByWeight::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  // flag age-length to Build age-length matrix for these years
  auto partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) (*category_iter)->age_length_->BuildAgeLengthMatrixForTheseYears(years_);
  }

  length_results_.resize(number_length_bins_ * category_labels_.size(), 0.0);
  weight_results_.resize(number_weight_bins_ * category_labels_.size(), 0.0);

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    auto process             = time_step->SubscribeToProcess(this, years_, process_label_);
    mortality_instantaneous_ = dynamic_cast<MortalityInstantaneous*>(process);
  }

  if (mortality_instantaneous_ == nullptr)
    LOG_FATAL() << "The observation " << label_ << " can be used with Process type " << PARAM_MORTALITY_INSTANTANEOUS << " only. Process " << process_label_
                << " was not found or has retained catch characteristics specified.";

  // Need to split the categories if any are combined for checking
  vector<string> temp_split_category_labels, split_category_labels;

  for (const string& category_label : category_labels_) {
    boost::split(temp_split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : temp_split_category_labels) {
      split_category_labels.push_back(split_category_label);
    }
  }

  // Need to make this a vector so it is compatible with the function
  vector<string> methods;
  methods.push_back(method_);

  // Do some checks so that the observation and process are compatible
  if (!mortality_instantaneous_->check_methods_for_removal_obs(methods))
    LOG_FATAL_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_
                                         << ". Check that the methods are compatible with this process";
  if (!mortality_instantaneous_->check_categories_in_methods_for_removal_obs(methods, split_category_labels))
    LOG_FATAL_P(PARAM_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_
                                  << ". Check that the categories are compatible with this process";
  if (!mortality_instantaneous_->check_years_in_methods_for_removal_obs(years_, methods))
    LOG_FATAL_P(PARAM_YEARS) << "could not find catches in all years in the instantaneous_mortality process labeled " << process_label_
                             << ". Check that the years are compatible with this process";

  fishery_ndx_to_get_catch_at_info_ = mortality_instantaneous_->get_fishery_ndx_for_catch_at(methods);
  vector<unsigned> category_ndxs    = mortality_instantaneous_->get_category_ndx_for_catch_at(split_category_labels);
  for (unsigned category_ndx = 0; category_ndx < split_category_labels.size(); ++category_ndx)
    category_lookup_for_ndx_to_get_catch_at_info_[split_category_labels[category_ndx]] = category_ndxs[category_ndx];
  year_ndx_to_get_catch_at_info_ = mortality_instantaneous_->get_year_ndx_for_catch_at(years_);

  length_weight_cv_adj_.resize(number_length_bins_);
  for (unsigned i = 0; i < number_length_bins_; ++i) {
    length_weight_cv_adj_[i] = length_weight_cv_ / length_bins_n_[i];
  }

  // set up length bins and weight bins with an extra value based on the last two bin values
  // for use in distribution2() with the plus group flag set to false
  length_plus_      = false;
  length_bins_plus_ = length_bins_;
  length_bins_plus_.push_back(length_bins_[number_length_bins_ - 1] + (length_bins_[number_length_bins_ - 1] - length_bins_[number_length_bins_ - 2]));

  weight_plus_      = false;
  weight_bins_plus_ = weight_bins_;
  weight_bins_plus_.push_back(weight_bins_[number_weight_bins_ - 1] + (weight_bins_[number_weight_bins_ - 1] - weight_bins_[number_weight_bins_ - 2]));

  auto age_data_size = model_->age_spread();
  partition_iter     = partition_->Begin();  // vector<vector<partition::Category> >
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    LOG_FINE() << "category: " << category_labels_[category_offset];
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      auto category_name = (*category_iter)->name_;
      LOG_FINE() << "category name: " << category_name;

      // this should go elsewhere; when are partitions initialised?
      if (map_age_weight_matrix_[category_labels_[category_offset]][category_name].size() == 0) {
        LOG_FINE() << "initialising age-length-weight matrices";

        map_age_length_matrix_[category_labels_[category_offset]][category_name].resize(age_data_size);
        map_age_weight_matrix_[category_labels_[category_offset]][category_name].resize(age_data_size);
        for (unsigned i = 0; i < age_data_size; ++i) {
          map_age_length_matrix_[category_labels_[category_offset]][category_name][i].resize(number_length_bins_, 0.0);
          map_age_weight_matrix_[category_labels_[category_offset]][category_name][i].resize(number_weight_bins_, 0.0);
        }

        map_length_weight_matrix_[category_labels_[category_offset]][category_name].resize(number_length_bins_);
        for (unsigned i = 0; i < number_length_bins_; ++i) {
          map_length_weight_matrix_[category_labels_[category_offset]][category_name][i].resize(number_weight_bins_, 0.0);
        }
      }
    }
  }

  // get the units specified in the age-length relationship (returns length_weight_->weight_units())
  // weight_units    = age_length->weight_units();
  string weight_units = model_->base_weight_units();  // since age_length->GetMeanWeight() returns the model weight units, per Craig Marsh 2021-07-07
  unit_multiplier_    = 1.0;
  // what value to multiply weight_units by to get units_
  if ((units_ == PARAM_TONNES) && (weight_units == PARAM_KGS || weight_units == PARAM_KILOGRAMS))
    unit_multiplier_ = 0.001;
  else if (units_ == PARAM_GRAMS && (weight_units == PARAM_KGS || weight_units == PARAM_KILOGRAMS))
    unit_multiplier_ = 1000;

  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (weight_units == PARAM_TONNES))
    unit_multiplier_ = 1000;
  else if (units_ == PARAM_GRAMS && (weight_units == PARAM_TONNES))
    unit_multiplier_ = 1000000;

  if ((units_ == PARAM_KGS || units_ == PARAM_KILOGRAMS) && (weight_units == PARAM_GRAMS))
    unit_multiplier_ = 0.0001;
  else if (units_ == PARAM_TONNES && (weight_units == PARAM_GRAMS))
    unit_multiplier_ = 0.0000001;
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProcessRemovalsByWeight::PreExecute() {
  LOG_FINEST() << "Entering observation " << label_;
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 * Execute the ProcessRemovalsByWeight expected values calculations
 */
void ProcessRemovalsByWeight::Execute() {
  LOG_TRACE();
  /**
   * Verify our cached partition and partition sizes are correct
   */
  //  auto categories = model_->categories();
  unsigned             year           = model_->current_year();
  std::pair<bool, int> this_year_iter = utilities::findInVector(years_, year);
  if (!this_year_iter.first)
    LOG_CODE_ERROR() << "if(!this_year_iter.first)";

  unsigned time_step      = model_->managers()->time_step()->current_time_step();
  auto     partition_iter = partition_->Begin();  // vector<vector<partition::Category> >

  Double mean_length;
  Double mean_weight;
  Double std_dev;

  // vector<Double> numbers_at_length(number_length_bins_);
  vector<Double> numbers_at_weight(number_weight_bins_);
  // vector<Double> expected_values_length(number_length_bins_);
  vector<Double> expected_values_weight(number_weight_bins_);

  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    LOG_FINE() << "category: " << category_labels_[category_offset];
    Double start_value   = 0.0;
    Double end_value     = 0.0;
    Double number_at_age = 0.0;

    // std::fill(expected_values_length.begin(), expected_values_length.end(), 0.0);
    std::fill(expected_values_weight.begin(), expected_values_weight.end(), 0.0);

    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      auto category_name = (*category_iter)->name_;
      LOG_FINE() << "category name: " << category_name;
      vector<Double>& Removals_at_age = mortality_instantaneous_->get_catch_at_by_year_fishery_category(
          year_ndx_to_get_catch_at_info_[this_year_iter.second], fishery_ndx_to_get_catch_at_info_[0], category_lookup_for_ndx_to_get_catch_at_info_[(*category_iter)->name_]);

      bool no_length_weight_change = true;

      for (unsigned j = 0; j < number_length_bins_; ++j) {
        // NOTE: hardcoded for now with minimum age (used to get cv[year][time_step][age])
        mean_weight = unit_multiplier_ * (*category_iter)->age_length_->mean_weight_by_length(length_bins_[j], (*category_iter)->min_age_, year, time_step);
        LOG_FINEST() << "Mean weight at length " << length_bins_[j] << " (CVs for age " << (*category_iter)->min_age_ << "): " << mean_weight
                     << " mean weight = " << (*category_iter)->age_length_->mean_weight_by_length(length_bins_[j], (*category_iter)->min_age_, year, time_step)
                     << " multiplier = " << unit_multiplier_;

        std_dev      = length_weight_cv_adj_[j] * mean_weight;
        auto tmp_vec = utilities::math::distribution2(weight_bins_plus_, weight_plus_, length_weight_distribution_, mean_weight, std_dev);

        // update the length-weight matrix only if necessary
        if (utilities::math::Sum(tmp_vec) > 0
            && utilities::math::Sum(map_length_weight_matrix_[category_labels_[category_offset]][category_name][j]) == utilities::math::Sum(tmp_vec)) {
          break;
        }

        no_length_weight_change = false;

        map_length_weight_matrix_[category_labels_[category_offset]][category_name][j] = tmp_vec;

        LOG_FINE() << "Fraction of weight_bin[0] at length " << length_bins_[j] << " for age " << (*category_iter)->min_age_ << ": "
                   << map_length_weight_matrix_[category_labels_[category_offset]][category_name][j][0] << " size "
                   << map_length_weight_matrix_[category_labels_[category_offset]][category_name][j].size();
        for (unsigned k = 0; k < number_weight_bins_; ++k) {
          LOG_FINEST() << "map_length_weight_matrix_[category_labels_[category_offset]][category_name]: fraction of weight " << weight_bins_[k] << " at length " << length_bins_[j]
                       << " (CVs for age " << (*category_iter)->min_age_ << "): " << map_length_weight_matrix_[category_labels_[category_offset]][category_name][j][k];
        }
      }

      LOG_FINE() << "number_length_bins_ " << number_length_bins_ << " number_weight_bins_ " << number_weight_bins_ << " age spread " << model_->age_spread();
      LOG_FINE() << "category data size (number of ages) " << (*category_iter)->data_.size();

      Double total_number_at_age = utilities::math::Sum(Removals_at_age);

      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        unsigned age = ((*category_iter)->min_age_ + data_offset);
        // Calculate the age structure removed from the fishing process
        number_at_age = Removals_at_age[data_offset];
        LOG_FINEST() << "Numbers at age " << age << ": " << number_at_age;

        mean_length = (*category_iter)->age_length_->mean_length(time_step, age);
        LOG_FINEST() << "Mean length at age " << age << ": " << mean_length;

        std_dev      = (*category_iter)->age_length_->cv(year, time_step, age) * mean_length;
        auto tmp_vec = utilities::math::distribution2(length_bins_plus_, length_plus_, (*category_iter)->age_length_->distribution(), mean_length, std_dev);

        // update the age-length (and the age-weight) matrix only if necessary
        if (no_length_weight_change && utilities::math::Sum(tmp_vec) > 0
            && utilities::math::Sum(map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset]) == utilities::math::Sum(tmp_vec)) {
          continue;
        }

        map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset] = tmp_vec;

        LOG_FINE() << "Fraction of length_bin[0] at age " << age << ": " << map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset][0] << " size "
                   << map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset].size();
        for (unsigned j = 0; j < number_length_bins_; ++j) {
          LOG_FINEST() << "map_age_length_matrix_[category_labels_[category_offset]][category_name]: fraction of length " << length_bins_[j] << " at age " << age << ": "
                       << map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset][j];
        }

        // Multiply by number_at_age
        // std::transform(map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset].begin(),
        // map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset].end(),
        // map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset].begin(), std::bind(std::multiplies<Double>(), std::placeholders::_1,
        // number_at_age));
        for (unsigned k = 0; k < number_weight_bins_; ++k) {
          Double tmp = 0.0;
          for (unsigned j = 0; j < number_length_bins_; ++j) {
            tmp += map_age_length_matrix_[category_labels_[category_offset]][category_name][data_offset][j]
                   * map_length_weight_matrix_[category_labels_[category_offset]][category_name][j][k];
          }
          map_age_weight_matrix_[category_labels_[category_offset]][category_name][data_offset][k] = tmp * number_at_age / total_number_at_age;
          LOG_FINEST() << "map_age_weight_matrix_[category_labels_[category_offset]][category_name][" << data_offset << "][" << k << "] = " << tmp << " * " << number_at_age
                       << " / " << total_number_at_age;
        }
        LOG_FINE() << "Fraction of weight_bin[0] at age " << age << ": " << map_age_weight_matrix_[category_labels_[category_offset]][category_name][data_offset][0] << " size "
                   << map_age_weight_matrix_[category_labels_[category_offset]][category_name][data_offset].size();
      }
      // numbers_at_weight.assign(map_age_weight_matrix_[category_labels_[category_offset]][category_name][0].size(), 0.0);
      std::fill(numbers_at_weight.begin(), numbers_at_weight.end(), 0.0);
      for (unsigned i = 0; i < map_age_weight_matrix_[category_labels_[category_offset]][category_name].size(); ++i) {
        for (unsigned j = 0; j < map_age_weight_matrix_[category_labels_[category_offset]][category_name][i].size(); ++j) {
          numbers_at_weight[j] += map_age_weight_matrix_[category_labels_[category_offset]][category_name][i][j];
        }
      }

      // for (unsigned length_offset = 0; length_offset < number_length_bins_; ++length_offset) {
      //   LOG_FINEST() << " numbers for length bin : " << length_bins_[length_offset] << " = " << numbers_at_length[length_offset];
      //   expected_values_length[length_offset] += numbers_at_length[length_offset];

      //   LOG_FINE() << "----------";
      //   LOG_FINE() << "Category: " << category_name << " at length " << length_bins_[length_offset];
      //   LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << numbers_at_length[length_offset];
      //   LOG_FINE() << "expected_value becomes: " << expected_values_length[length_offset];
      // }

      for (unsigned weight_offset = 0; weight_offset < number_weight_bins_; ++weight_offset) {
        LOG_FINEST() << "numbers for weight bin : " << weight_bins_[weight_offset] << " = " << numbers_at_weight[weight_offset];
        expected_values_weight[weight_offset] += numbers_at_weight[weight_offset];

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << category_name << " at weight " << weight_bins_[weight_offset];
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << numbers_at_weight[weight_offset];
        LOG_FINE() << "expected_value becomes: " << expected_values_weight[weight_offset];
      }
    }

    if (expected_values_weight.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values_weight.size(" << expected_values_weight.size() << ") != proportions_[category_offset].size("
                       << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    for (unsigned i = 0; i < expected_values_weight.size(); ++i) {
      SaveComparison(category_labels_[category_offset], 0, weight_bins_[i], expected_values_weight[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProcessRemovalsByWeight::CalculateScore() {
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
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:" << scores_[year];
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
