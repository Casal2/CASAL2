/**
 * @file ProcessRemovalsByLength.cpp
 * @author  C Marsh
 * @version 1.0
 * @date 12.8.15
 * @section LICENSE
 *
 * Copyright NIWA Science 2016 - www.niwa.co.nz
 */

// Headers
#include "ProcessRemovalsByLength.h"

#include <algorithm>

#include "Common/Model/Model.h"
#include "Age/AgeLengths/AgeLength.h"
#include "Common/Categories/Categories.h"
#include "Common/Partition/Accessors/All.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/Map.h"
#include "Common/Utilities/Math.h"
#include "Common/Utilities/To.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

// Namespaces
namespace niwa {
namespace age {
namespace observations {

/**
 * Default constructor
 */
ProcessRemovalsByLength::ProcessRemovalsByLength(Model* model)
  : Observation(model) {

  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<Double>(PARAM_LENGTH_BINS, &length_bins_, "Length bins", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute in", "");
  parameters_.Bind<bool>(PARAM_LENGTH_PLUS, &length_plus_, "Is the last bin a plus group", "", true);
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tolerance_, "Tolerance for rescaling proportions", "", Double(0.001));
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "the value of process error", "", true);
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "Label of observed method of removals", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "Table of error values of the observed values (note the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_MORTALITY_INSTANTANEOUS_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation", "");

  mean_proportion_method_ = false;

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
}

/**
 * Destructor
 */
ProcessRemovalsByLength::~ProcessRemovalsByLength() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProcessRemovalsByLength::DoValidate() {
  // How many elements are expected in our observed table;
  if(length_plus_) {
    number_bins_ = length_bins_.size();
  } else {
    number_bins_ = length_bins_.size() - 1;
  }
  for (auto year : years_) {
  	if((year < model_->start_year()) || (year > model_->final_year()))
  		LOG_ERROR_P(PARAM_YEARS) << "Years can't be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << "). Please fix this.";
  }

  map<unsigned, vector<Double>> error_values_by_year;
  map<unsigned, vector<Double>> obs_by_year;

  /**
   * Do some simple checks
   * e.g Validate that the length_bins are strictly increasing
   */
  for(unsigned length = 0; length < length_bins_.size(); ++length) {
    if(length_bins_[length] < 0.0)
    if(length_bins_[length] > length_bins_[length + 1])
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bins must be strictly increasing " << length_bins_[length] << " is greater than " << length_bins_[length +1];
  }

  if (process_error_values_.size() != 0 && process_error_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provied (" << process_error_values_.size() << ") does not match the number of years provided ("
        << years_.size() << ")";
  }
  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }
  if (process_error_values_.size() != 0)
    process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);
  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << AS_DOUBLE(delta_) << ") cannot be less than 0.0";

  /**
   * Validate the number of obs provided matches age spread * category_labels * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned obs_expected = number_bins_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      Double value = 0;
      if (!utilities::To<Double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_OBS) << "you supplied " << obs_by_year[year].size() << " lengths, but we expected " << obs_expected -1 << " can you please sort this out. Chairs";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_FATAL_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";
    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      Double value = 0;

      if (!utilities::To<Double>(error_values_data_line[i], value))
        LOG_FATAL_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a double";
      if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << AS_DOUBLE(value) << ") cannot be equal to or less than 0.0";
      } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << AS_DOUBLE(value) << ") cannot be less than 0.0";
      }

      error_values_by_year[year].push_back(value);
    }
    if (error_values_by_year[year].size() == 1) {
      error_values_by_year[year].assign(obs_expected - 1, error_values_by_year[year][0]);
    }
    if (error_values_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_ERROR_VALUES) << "We counted " << error_values_by_year[year].size() << " error values by year but expected " << obs_expected -1 << " based on the obs table";
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
      for (unsigned j = 0; j < number_bins_; ++j) {
        unsigned obs_index = i * number_bins_ + j;
        value = iter->second[obs_index];

        Double error_value = error_values_by_year[iter->first][obs_index];
        error_values_[iter->first][category_labels_[i]].push_back(error_value);
        proportions_[iter->first][category_labels_[i]].push_back(value);
        total += value;
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
void ProcessRemovalsByLength::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

//  if (ageing_error_label_ != "")
//   LOG_CODE_ERROR() << "ageing error has not been implemented for the proportions at age observation";

  length_results_.resize(number_bins_ * category_labels_.size(), 0.0);

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    auto process = time_step->SubscribeToProcess(this, years_, process_label_);
    mortality_instantaneous_ = dynamic_cast<MortalityInstantaneous*>(process);
  }

  if (!mortality_instantaneous_)
    LOG_ERROR_P(PARAM_PROCESS) << "This observation can only be used for Process of type = " << PARAM_MORTALITY_INSTANTANEOUS;

  // Need to split the categories if any are combined for checking
  vector<string> temp_split_category_labels, split_category_labels;

  for (const string& category_label : category_labels_) {
    boost::split(temp_split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : temp_split_category_labels) {
      split_category_labels.push_back(split_category_label);
    }
  }

  // Need to make this a vector so its compatible with the function couldn't be bothered templating sorry
  vector<string> methods;
  methods.push_back(method_);
  // Do some checks so that the observation and process are compatible
  if (!mortality_instantaneous_->check_methods_for_removal_obs(methods))
    LOG_ERROR_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_ << " please check that the methods are compatible with this process";
  if (!mortality_instantaneous_->check_categories_in_methods_for_removal_obs(methods, split_category_labels))
    LOG_ERROR_P(PARAM_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_ << " please check that the categories are compatible with this process";
  if (!mortality_instantaneous_->check_years_in_methods_for_removal_obs(years_, methods))
    LOG_ERROR_P(PARAM_YEARS) << "could not find catches in all years in the instantaneous_mortality process labeled " << process_label_ << " please check that the years are compatible with this process";

}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProcessRemovalsByLength::PreExecute() {
  LOG_FINEST() << "Entering observation " << label_;

  cached_partition_->BuildCache();

  if (cached_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 *
 */
void ProcessRemovalsByLength::Execute() {
  LOG_TRACE();
  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto categories = model_->categories();
  unsigned year = model_->current_year();
  unsigned time_step = model_->managers().time_step()->current_time_step();
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // vector<vector<partition::Category> >
  map<unsigned,map<string, map<string, vector<Double>>>> &Removals_at_age = mortality_instantaneous_->catch_at();

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each length using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    LOG_FINEST() << "category: " << category_labels_[category_offset];
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;
    Double      number_at_age      = 0.0;

    vector<Double>              expected_values(number_bins_, 0.0);
    vector<Double>              numbers_at_length;
    vector<vector<Double>>      age_length_matrix_;

    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */


    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      AgeLength* age_length = categories->age_length((*category_iter)->name_);

      age_length_matrix_.resize((*category_iter)->data_.size());

      vector<Double> age_frequencies(length_bins_.size(), 0.0);
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        unsigned age        = ( (*category_iter)->min_age_ + data_offset);

        // Calculate the age structure removed from the fishing process
        number_at_age = Removals_at_age[year][method_][(*category_iter)->name_][data_offset];
        LOG_FINEST() << "Numbers at age = " << age << " = " <<  number_at_age << " start value : "<< start_value << " end value : " << end_value;
        // Implement an algorithm similar to DoAgeLengthConversion() to convert numbers at age to numbers at length
        // This is different to DoAgeLengthConversion as this number is now not related to the partition
        Double mu= (*category_iter)->mean_length_by_time_step_age_[time_step][age];

        LOG_FINEST() << "mean = " << mu << " cv = " << age_length->cv(year, time_step, age) << " distribution = " << age_length->distribution() << " and length plus group = " << length_plus_;
        age_length->CummulativeNormal(mu, age_length->cv(year, time_step, age), age_frequencies, length_bins_, length_plus_);

        age_length_matrix_[data_offset].resize(number_bins_);

        // Loop through the length bins and multiple the partition of the current age to go from
        // length frequencies to age length numbers
          for (unsigned j = 0; j < number_bins_; ++j) {
            age_length_matrix_[data_offset][j] = number_at_age * age_frequencies[j];
            LOG_FINEST() << "The proportion of fish in length bin : " << length_bins_[j] << " = "<< age_frequencies[j];
          }
      }

         if (age_length_matrix_.size() == 0)
            LOG_CODE_ERROR() << "if (age_length_matrix_.size() == 0)";

          numbers_at_length.assign(age_length_matrix_[0].size(), 0.0);
          for (unsigned i = 0; i < age_length_matrix_.size(); ++i) {
            for (unsigned j = 0; j < age_length_matrix_[i].size(); ++j) {
              numbers_at_length[j] += age_length_matrix_[i][j];
            }
          }



            for (unsigned length_offset = 0; length_offset < number_bins_; ++length_offset) {
              LOG_FINEST() << " numbers for length bin : " << length_bins_[length_offset] << " = " <<  numbers_at_length[length_offset];
             expected_values[length_offset] += numbers_at_length[length_offset];

              LOG_FINE() << "----------";
              LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length " << length_bins_[length_offset];
              LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << numbers_at_length[length_offset];
              LOG_FINE() << "expected_value becomes: " << expected_values[length_offset];
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
void ProcessRemovalsByLength::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
	LOG_FINEST() << "Calculating score for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
    for (auto& iter :  comparisons_) {
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
          comparison.expected_  = comparison.expected_ / running_total;
        else
          comparison.expected_  = 0.0;
      }
    }
    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation score calculation";
      LOG_FINEST() << "[" << year << "] Initial Score:"<< scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "]+ likelihood score: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */
