/**
 * @file ProcessRemovalsByWeight.cpp
 *
 * @section LICENSE
 *
 * Copyright NIWA Science 2021 - www.niwa.co.nz
 */

// Headers
#include "ProcessRemovalsByWeight.h"

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <iterator>

#include "Model/Model.h"
#include "AgeLengths/AgeLength.h"
#include "Categories/Categories.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
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
ProcessRemovalsByWeight::ProcessRemovalsByWeight(Model* model) :
    Observation(model) {

  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<string>(PARAM_MORTALITY_INSTANTANEOUS_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation", "");
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "The label of observed method of removals", "", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to execute in", "");
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "The tolerance for rescaling proportions", "", double(0.001))->set_range(0.0, 1.0, false, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<double>(PARAM_LENGTH_WEIGHT_CV, &length_weight_cv_, "The CV for the length-weight relationship", "", double(0.10))->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_LENGTH_WEIGHT_DISTRIBUTION, &length_weight_distribution_label_, "The distribution of the length-weight relationship", "", PARAM_NORMAL)->set_allowed_values({PARAM_NORMAL, PARAM_LOGNORMAL});
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "");
  parameters_.Bind<double>(PARAM_LENGTH_BINS_N, &length_bins_n_, "The average number in each length bin", "");
  parameters_.Bind<string>(PARAM_UNITS, &units_, "The units for the weight bins", "grams, kgs or tonnes", PARAM_KGS)->set_allowed_values({PARAM_GRAMS, PARAM_TONNES, PARAM_KGS});
  parameters_.Bind<double>(PARAM_FISHBOX_WEIGHT, &fishbox_weight_, "The target weight of each box", "", double(20.0))->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_WEIGHT_BINS, &weight_bins_, "The weight bins", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note that the units depend on the likelihood)", "", false);

  mean_proportion_method_ = false;

  RegisterAsAddressable(PARAM_PROCESS_ERRORS, &process_error_values_);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
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
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year()
        << "), or greater than final_year (" << model_->final_year() << ").";
  }

  map<unsigned, vector<Double>> error_values_by_year;
  map<unsigned, vector<Double>> obs_by_year;

  /**
   * Do some simple checks
   * e.g., validate that the length and weight bins are strictly increasing
   */

  vector<double> model_length_bins = model_->length_bins();
  if (length_bins_[0] < model_length_bins[0])
    LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin value " << length_bins_[0] << " is smaller than the smallest model length bin " << model_length_bins[0];
  if (length_bins_[(number_length_bins_ - 1)] > model_length_bins[(model_length_bins.size() - 1)])
    LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin value " << length_bins_[(number_length_bins_ - 1)] << " is larger than the largest model length bin " << model_length_bins[(model_length_bins.size() - 1)];

  if (length_bins_.size() != length_bins_n_.size())
    LOG_ERROR_P(PARAM_LENGTH_BINS_N) << ": The number of length bins " << length_bins_.size() << " does not match the number of length bin number values " << length_bins_n_.size();

  for (unsigned length = 0; length < length_bins_.size(); ++length) {
    if (length_bins_[length] < 0.0)
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin values must be positive: " << length_bins_[length] << " is less than 0.0";

    if (length > 0 && length_bins_[length - 1] >= length_bins_[length])
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bin values must be strictly increasing: " << length_bins_[length - 1]
        << " is greater than or equal to " << length_bins_[length];
  }

  for (unsigned length = 0; length < length_bins_n_.size(); ++length) {
    if (length_bins_n_[length] <= 0.0)
      LOG_ERROR_P(PARAM_LENGTH_BINS_N) << ": Length bin number values must be positive: " << length_bins_n_[length] << " is less than or equal to 0.0";
  }

  for (unsigned weight = 0; weight < weight_bins_.size(); ++weight) {
    if (weight_bins_[weight] < 0.0)
      LOG_ERROR_P(PARAM_WEIGHT_BINS) << ": Weight bin values must be positive: " << weight_bins_[weight] << " is less than 0.0";

    if (weight > 0 && weight_bins_[weight - 1] >= weight_bins_[weight])
      LOG_ERROR_P(PARAM_WEIGHT_BINS) << ": Weight bin values must be strictly increasing: " << weight_bins_[weight - 1]
        << " is greater than or equal to " << weight_bins_[weight];
  }


  if (process_error_values_.size() != 0 && process_error_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provided (" << process_error_values_.size()
      << ") does not match the number of years provided (" << years_.size() << ")";
  }
  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }
  if (process_error_values_.size() != 0) {
    if (process_error_values_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_PROCESS_ERRORS) << "Supply a process error for each year. Values for " << process_error_values_.size()
        << " years were provided, but " << years_.size() << " years are required";
    }
    process_errors_by_year_ = utilities::Map<Double>::create(years_, process_error_values_);
  } else {
    Double process_val = 0.0;
    process_errors_by_year_ = utilities::Map<Double>::create(years_, process_val);
  }

  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";

  /**
   * Validate the number of obs provided matches age spread * category_labels * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned obs_expected = number_weight_bins_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but " << years_.size()
      << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but " << obs_expected
        << " should match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      Double value = 0.0;
      if (!utilities::To<Double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a Double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_OBS)<< " " << obs_by_year[year].size() << " weights were provided, but " << obs_expected -1
        << "weights are required";
    }

    /**
     * Build our error value map
     */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_FATAL_P(PARAM_ERROR_VALUES)<< " has " << error_values_data.size() << " rows defined, but " << years_.size()
      << " to match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
        << " to match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;

    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_FATAL_P(PARAM_ERROR_VALUES)<< " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_FATAL_P(PARAM_ERROR_VALUES)<< " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      Double value = 0.0;
      if (!utilities::To<Double>(error_values_data_line[i], value))
        LOG_FATAL_P(PARAM_ERROR_VALUES)<< " value (" << error_values_data_line[i] << ") could not be converted to a Double";
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
      LOG_FATAL_P(PARAM_ERROR_VALUES)<< " " << error_values_by_year[year].size() << " error values by year were provided, but "
        << obs_expected -1 << " values are required based on the obs table";
    }

    /**
     * Build our proportions and error values for use in the observation
     * If the proportions for a given observation do not sum to 1.0
     * and is off by more than the tolerance rescale them.
     */
  Double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    Double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < number_weight_bins_; ++j) {
        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end())
        {
          unsigned obs_index = i * number_weight_bins_ + j;
          value = iter->second[obs_index];
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          proportions_[iter->first][category_labels_[i]].push_back(value);
          total += value;
        }
      }
    }

    if (fabs(1.0 - total) > tolerance_) {
      LOG_ERROR_P(PARAM_OBS) << ": obs sum total (" << total << ") for year (" << iter->first << ") exceeds tolerance (" << tolerance_ << ") from 1.0";
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProcessRemovalsByWeight::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

//  if (ageing_error_label_ != "")
//   LOG_CODE_ERROR() << "ageing error has not been implemented for the proportions at age observation";

  length_results_.resize(number_length_bins_ * category_labels_.size(), 0.0);
  weight_results_.resize(number_weight_bins_ * category_labels_.size(), 0.0);

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    auto process = time_step->SubscribeToProcess(this, years_, process_label_);
    mortality_instantaneous_ = dynamic_cast<MortalityInstantaneous*>(process);
  }

  if (mortality_instantaneous_ == nullptr)
    LOG_FATAL() << "Observation " << label_ << " can be used with Process type " << PARAM_MORTALITY_INSTANTANEOUS
      << " only. Process " << process_label_ << " was not found or has retained catch characteristics specified.";


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
    LOG_ERROR_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the methods are compatible with this process";
  if (!mortality_instantaneous_->check_categories_in_methods_for_removal_obs(methods, split_category_labels))
    LOG_ERROR_P(PARAM_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the categories are compatible with this process";
  if (!mortality_instantaneous_->check_years_in_methods_for_removal_obs(years_, methods))
    LOG_ERROR_P(PARAM_YEARS) << "could not find catches in all years in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the years are compatible with this process";


  auto data_size = model_->age_spread();
  auto partition_iter = partition_->Begin(); // vector<vector<partition::Category> >
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    LOG_FINE() << "category: " << category_labels_[category_offset];

    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {

      vector<vector<Double>> age_length_matrix;
      vector<vector<Double>> length_weight_matrix;
      vector<vector<Double>> age_weight_matrix;

      age_length_matrix.resize(data_size);
      age_weight_matrix.resize(data_size);
      for (unsigned i = 0; i < data_size; ++i) {
        age_length_matrix[i].resize(number_length_bins_);
        age_weight_matrix[i].resize(number_weight_bins_);
      }

      length_weight_matrix.resize(number_length_bins_);
      for (unsigned i = 0; i < number_length_bins_; ++i) {
        length_weight_matrix[i].resize(number_weight_bins_);
      }

      map_age_length_matrix_[category_labels_[category_offset]][(*category_iter)->name_]    = age_length_matrix;
      map_length_weight_matrix_[category_labels_[category_offset]][(*category_iter)->name_] = length_weight_matrix;
      map_age_weight_matrix_[category_labels_[category_offset]][(*category_iter)->name_]    = age_weight_matrix;
    }
  }

  length_weight_cv_adj_.resize(number_length_bins_);
  for (unsigned i = 0; i < number_length_bins_; ++i) {
    length_weight_cv_adj_[i] = length_weight_cv_ / length_bins_n_[i];
  }

  // set up length bins and weight bins with an extra value based on the last two bin values
  // for use in distribution2() with the plus group flag set to false
  length_plus_ = false;
  length_bins_plus_ = length_bins_;
  length_bins_plus_.push_back(length_bins_[number_length_bins_ - 1] + (length_bins_[number_length_bins_ - 1] - length_bins_[number_length_bins_ - 2]));

  weight_plus_ = false;
  weight_bins_plus_ = weight_bins_;
  weight_bins_plus_.push_back(weight_bins_[number_weight_bins_ - 1] + (weight_bins_[number_weight_bins_ - 1] - weight_bins_[number_weight_bins_ - 2]));
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

  cached_partition_->BuildCache();

  if (cached_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR()<< "cached_partition_->Size() != proportions_[model->current_year()].size()";
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR()<< "partition_->Size() != proportions_[model->current_year()].size()";
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
  unsigned year       = model_->current_year();
  unsigned time_step  = model_->managers().time_step()->current_time_step();

  auto cached_partition_iter = cached_partition_->Begin();
  auto partition_iter        = partition_->Begin(); // vector<vector<partition::Category> >
  map<unsigned, map<string, map<string, vector<Double>>>> &Removals_at_age = mortality_instantaneous_->catch_at();

  Double mean_length;
  Double mean_weight;
  Double std_dev;
  string weight_units;
  Double unit_multiplier;

  // vector<Double> numbers_at_length(number_length_bins_);
  vector<Double> numbers_at_weight(number_weight_bins_);
  // vector<Double> expected_values_length(number_length_bins_);
  vector<Double> expected_values_weight(number_weight_bins_);

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each weight using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    LOG_FINE() << "category: " << category_labels_[category_offset];
    Double start_value   = 0.0;
    Double end_value     = 0.0;
    Double number_at_age = 0.0;

    // std::fill(expected_values_length.begin(), expected_values_length.end(), 0.0);
    std::fill(expected_values_weight.begin(), expected_values_weight.end(), 0.0);

    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */
    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {

      vector<vector<Double>>& age_length_matrix    = map_age_length_matrix_[category_labels_[category_offset]][(*category_iter)->name_];
      vector<vector<Double>>& length_weight_matrix = map_length_weight_matrix_[category_labels_[category_offset]][(*category_iter)->name_];
      vector<vector<Double>>& age_weight_matrix    = map_age_weight_matrix_[category_labels_[category_offset]][(*category_iter)->name_];


      AgeLength* age_length = (*category_iter)->age_length_;

      weight_units    = age_length->weight_units();
      unit_multiplier = 1.0;
      // what value to multiply weight_units by to get units_
      if ((units_ == PARAM_TONNES) && (weight_units == PARAM_KGS))
        unit_multiplier = 0.001;
      else if (units_ == PARAM_GRAMS && (weight_units == PARAM_KGS))
        unit_multiplier = 1000;

      if ((units_ == PARAM_KGS) && (weight_units == PARAM_TONNES))
        unit_multiplier = 1000;
      else if (units_ == PARAM_GRAMS && (weight_units == PARAM_TONNES))
        unit_multiplier = 1000000;

      if ((units_ == PARAM_KGS) && (weight_units == PARAM_GRAMS))
        unit_multiplier = 0.0001;
      else if (units_ == PARAM_TONNES && (weight_units == PARAM_GRAMS))
        unit_multiplier = 0.0000001;
      LOG_FINE() << "category " << (*category_iter)->name_ << " unit multiplier " << unit_multiplier << " from " << weight_units << " to " << units_;


      for (unsigned j = 0; j < number_length_bins_; ++j) {
        // NOTE: hardcoded for now with minimum age (used to get cv[year][time_step][age])
        mean_weight = unit_multiplier * age_length->GetMeanWeight(year, time_step, (*category_iter)->min_age_, length_bins_[j]);
        LOG_FINEST() << "Mean weight at length " << length_bins_[j] << " (CVs for age " << (*category_iter)->min_age_ << "): " << mean_weight;

        std_dev = length_weight_cv_adj_[j] * mean_weight;
        auto tmp_vec = utilities::math::distribution2(weight_bins_plus_, weight_plus_, length_weight_distribution_, mean_weight, std_dev);

        // update the length-weight matrix only if necessary
        if (utilities::math::Sum(tmp_vec) > 0 && utilities::math::Sum(length_weight_matrix[j]) == utilities::math::Sum(tmp_vec)) {
          break;
        }

        length_weight_matrix[j] = tmp_vec;

        LOG_FINE() << "Fraction of weight_bin[0] at length " << length_bins_[j] << " for age " << (*category_iter)->min_age_ << ": " << length_weight_matrix[j][0] << " size " << length_weight_matrix[j].size();
        for (unsigned k = 0; k < number_weight_bins_; ++k) {
          LOG_FINEST() << "length_weight_matrix: fraction of weight " << weight_bins_[k] << " at length " << length_bins_[j] << " (CVs for age " << (*category_iter)->min_age_ << "): " << length_weight_matrix[j][k];
        }
      }


      LOG_FINE() << "number_length_bins_ " << number_length_bins_ << " number_weight_bins_ " << number_weight_bins_ << " age spread " << model_->age_spread();
      LOG_FINE() << "category data size (number of ages) " << (*category_iter)->data_.size();

      Double total_number_at_age = utilities::math::Sum(Removals_at_age[year][method_][(*category_iter)->name_]);

      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        unsigned age = ((*category_iter)->min_age_ + data_offset);
        // Calculate the age structure removed from the fishing process
        number_at_age = Removals_at_age[year][method_][(*category_iter)->name_][data_offset];
        LOG_FINEST() << "Numbers at age " << age << ": " << number_at_age;

        mean_length = age_length->GetMeanLength(year, time_step, age);
        LOG_FINEST() << "Mean length at age " << age << ": " << mean_length;

        std_dev = age_length->cv(year, time_step, age) * mean_length;
        auto tmp_vec = utilities::math::distribution2(length_bins_plus_, length_plus_, age_length->distribution(), mean_length, std_dev);

        // update the age-length (and the age-weight) matrix only if necessary
        if (utilities::math::Sum(tmp_vec) > 0 && utilities::math::Sum(age_length_matrix[data_offset]) == utilities::math::Sum(tmp_vec)) {
          break;
        }

        age_length_matrix[data_offset] = tmp_vec;

        LOG_FINE() << "Fraction of length_bin[0] at age " << age << ": " << age_length_matrix[data_offset][0] << " size " << age_length_matrix[data_offset].size();
        for (unsigned j = 0; j < number_length_bins_; ++j) {
          LOG_FINEST() << "age_length_matrix: fraction of length " << length_bins_[j] << " at age " << age << ": " << age_length_matrix[data_offset][j];
        }

        // Multiply by number_at_age
        // std::transform(age_length_matrix[data_offset].begin(), age_length_matrix[data_offset].end(), age_length_matrix[data_offset].begin(), std::bind(std::multiplies<Double>(), std::placeholders::_1, number_at_age));
        for (unsigned k = 0; k < number_weight_bins_; ++k) {
          Double tmp = 0.0;
          for (unsigned j = 0; j < number_length_bins_; ++j) {
            tmp += age_length_matrix[data_offset][j] * length_weight_matrix[j][k];
          }
          age_weight_matrix[data_offset][k] = tmp * number_at_age / total_number_at_age;
          LOG_FINEST() << "age_weight_matrix[" << data_offset << "][" << k << "] = " << tmp << " * " << number_at_age << " / " << total_number_at_age;
        }
        LOG_FINE() << "Fraction of weight_bin[0] at age " << age << ": " << age_weight_matrix[data_offset][0] << " size " << age_weight_matrix[data_offset].size();
      }


      // if (age_length_matrix.size() == 0)
      //   LOG_CODE_ERROR()<< "if (age_length_matrix_.size() == 0)";

      // numbers_at_length.assign(age_length_matrix[0].size(), 0.0);
      // for (unsigned i = 0; i < age_length_matrix.size(); ++i) {
      //   for (unsigned j = 0; j < age_length_matrix[i].size(); ++j) {
      //     numbers_at_length[j] += age_length_matrix[i][j];
      //   }
      // }

      // numbers_at_weight.assign(age_weight_matrix[0].size(), 0.0);
      std::fill(numbers_at_weight.begin(), numbers_at_weight.end(), 0.0);
      for (unsigned i = 0; i < age_weight_matrix.size(); ++i) {
        for (unsigned j = 0; j < age_weight_matrix[i].size(); ++j) {
          numbers_at_weight[j] += age_weight_matrix[i][j];
        }
      }


      // for (unsigned length_offset = 0; length_offset < number_length_bins_; ++length_offset) {
      //   LOG_FINEST() << " numbers for length bin : " << length_bins_[length_offset] << " = " << numbers_at_length[length_offset];
      //   expected_values_length[length_offset] += numbers_at_length[length_offset];

      //   LOG_FINE() << "----------";
      //   LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length " << length_bins_[length_offset];
      //   LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << numbers_at_length[length_offset];
      //   LOG_FINE() << "expected_value becomes: " << expected_values_length[length_offset];
      // }

      for (unsigned weight_offset = 0; weight_offset < number_weight_bins_; ++weight_offset) {
        LOG_FINEST() << " numbers for weight bin : " << weight_bins_[weight_offset] << " = " << numbers_at_weight[weight_offset];
        expected_values_weight[weight_offset] += numbers_at_weight[weight_offset];

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << (*category_iter)->name_ << " at weight " << weight_bins_[weight_offset];
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << numbers_at_weight[weight_offset];
        LOG_FINE() << "expected_value becomes: " << expected_values_weight[weight_offset];
      }
    }


    if (expected_values_weight.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR()<< "expected_values_weight.size(" << expected_values_weight.size() << ") != proportions_[category_offset].size("
      << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values_weight.size(); ++i) {
      SaveComparison(category_labels_[category_offset], 0, weight_bins_[i], expected_values_weight[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i],
                     0.0, delta_, 0.0);
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
      Double total = 0.0;
      for (auto& comparison : iter.second)
        total += comparison.observed_;
      for (auto& comparison : iter.second)
        comparison.observed_ /= total;
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
