/**
 * @file ProportionsMatureByAge.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 12/7/2017
 * @section LICENSE
 *
 * Copyright NIWA Science 2017 - www.niwa.co.nz
 *
 */

// Headers
#include "ProportionsMatureByAge.h"

#include <algorithm>

#include "Model/Model.h"
#include "AgeingErrors/AgeingError.h"
#include "AgeingErrors/Manager.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Categories/Categories.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
// Namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProportionsMatureByAge::ProportionsMatureByAge(Model* model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of time-step that the observation occurs in", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Use age plus group", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of proportions at age mature ", "", false);
  // TODO:  is tolerance missing?
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "Table of error values of the observed values (note the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_TOTAL_CATEGORIES, &total_category_labels_, "All category labels that were vulnerable to sampling at the time of this observation (not including the categories already given)", "", true);
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the mortality block of the time step when the observation is evaluated", "", double(0.5));

  mean_proportion_method_ = false;

  allowed_likelihood_types_.push_back(PARAM_BINOMIAL);

}

/**
 * Destructor
 */
ProportionsMatureByAge::~ProportionsMatureByAge() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate total_categories command
 */
void ProportionsMatureByAge::DoValidate() {
  age_spread_ = (max_age_ - min_age_) + 1;

  map<unsigned, vector<Double>> error_values_by_year;
  map<unsigned, vector<double>> obs_by_year;

  /**
   * Now go through each category and split it if required, then check each piece to ensure
   * it's a valid category
   */
  Categories* categories = model_->categories();
  LOG_FINEST() << "Number of different total categories " << total_category_labels_.size();
  vector<string> split_category_labels;
  for (const string& category_label : total_category_labels_) {
    boost::split(split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : split_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }
  for (auto tot_category : total_category_labels_) {
    if(std::find(category_labels_.begin(),category_labels_.end(),tot_category) != category_labels_.end())
      LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << "category '" << tot_category << "' was found in the parameter " <<  PARAM_CATEGORIES << ". Please remove it from the " << PARAM_TOTAL_CATEGORIES << " parameter";

  }
  LOG_FINEST() << "Number of different total categories after splitting " << total_category_labels_.size();

  /**
   * Do some simple checks
   */
  if (min_age_ < model_->min_age())
    LOG_ERROR_P(PARAM_MIN_AGE) << ": min_age (" << min_age_ << ") is less than the model's min_age (" << model_->min_age() << ")";
  if (max_age_ > model_->max_age())
    LOG_ERROR_P(PARAM_MAX_AGE) << ": max_age (" << max_age_ << ") is greater than the model's max_age (" << model_->max_age() << ")";
  if (process_error_values_.size() != 0 && process_error_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provied (" << process_error_values_.size() << ") does not match the number of years provided ("
        << years_.size() << ")";
  }
  for (auto year : years_) {
  	if((year < model_->start_year()) || (year > model_->final_year()))
  		LOG_ERROR_P(PARAM_YEARS) << "Years can't be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << "). Please fix this.";
  }

  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }
  if (process_error_values_.size() != 0) {
    if (process_error_values_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_PROCESS_ERRORS) << "need to supply a process error for each year, you supplied '" << process_error_values_.size() << "', but you need to supply '" << years_.size() << "'";
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
  unsigned obs_expected = age_spread_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age speard * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a double";
      // TODO:  need additional proportion checks
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << obs_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    LOG_FINEST() << "cols = " << error_values_data_line.size();
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age speard * categories + 1 (for year)";
    }
    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";
    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      Double value = 0.0;
      if (!utilities::To<Double>(error_values_data_line[i], value))
        LOG_ERROR_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a double";
      if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be equal to or less than 0.0";
      } else if ((likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) || (likelihood_type_ == PARAM_DIRICHLET && value < 0.0)) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
      }

      error_values_by_year[year].push_back(value);
    }
    if (error_values_by_year[year].size() == 1) {
      error_values_by_year[year].assign(obs_expected - 1, error_values_by_year[year][0]);
    }
    if (error_values_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << error_values_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")";
  }

  /**
   * Build our proportions and error values for use in the observation
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;
        if (!utilities::To<double>(iter->second[obs_index], value))
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1
              << " in the definition could not be converted to a numeric double";

        Double error_value = error_values_by_year[iter->first][obs_index];
        error_values_[iter->first][category_labels_[i]].push_back(error_value);
        proportions_[iter->first][category_labels_[i]].push_back(value);
      }
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProportionsMatureByAge::DoBuild() {
  LOG_TRACE();
  // Get all categories in the system.
  total_partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, total_category_labels_));
  total_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, total_category_labels_));

  // all categories
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

  // Create a pointer to misclassification matrix
  if( ageing_error_label_ != "") {
  ageing_error_ = model_->managers().ageing_error()->GetAgeingError(ageing_error_label_);
  if (!ageing_error_)
    LOG_ERROR_P(PARAM_AGEING_ERROR) << "(" << ageing_error_label_ << ") could not be found. Have you defined it?";
  }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  TimeStep* time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToBlock(this, year);
  }
}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProportionsMatureByAge::PreExecute() {
  LOG_TRACE();
  cached_partition_->BuildCache();
  total_cached_partition_->BuildCache();
  LOG_FINEST() << "Entering observation " << label_;

  if (cached_partition_->Size() != proportions_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  }
  if (total_cached_partition_->Size() != proportions_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "total_cached_partition_->Size() != proportions_[model->current_year()].size()";

  }
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
  if (total_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "total_partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 *
 */
void ProportionsMatureByAge::Execute() {
  LOG_TRACE();

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin();
  auto total_cached_partition_iter  = total_cached_partition_->Begin();
  auto total_partition_iter         = total_partition_->Begin();
  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  LOG_FINEST() << "Number of categories " << category_labels_.size();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;


    vector<Double> expected_values(age_spread_, 0.0);
    vector<Double> numbers_age((model_->age_spread() + 1), 0.0);
    vector<Double> total_numbers_age((model_->age_spread() + 1), 0.0);
    /**
     * Loop through the total categories building up numbers at age.
     */
    auto total_category_iter = total_partition_iter->begin();
    auto total_cached_category_iter = total_cached_partition_iter->begin();
    for (; total_category_iter != total_partition_iter->end(); ++total_cached_category_iter, ++total_category_iter) {
      for (unsigned data_offset = 0; data_offset < (*total_category_iter)->data_.size(); ++data_offset) {
        // We now need to loop through all ages to apply ageing misclassification matrix to account
        // for ages older than max_age_ that could be classified as an individual within the observation range
        unsigned age = ( (*total_category_iter)->min_age_ + data_offset);

        start_value   = (*total_cached_category_iter).data_[data_offset];
        end_value     = (*total_category_iter)->data_[data_offset];

        total_numbers_age[data_offset] += start_value + (end_value - start_value) * time_step_proportion_;

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << (*total_category_iter)->name_ << " at age " << age;
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value;
      }
    }
    /**
     * Loop through the categories building up numbers at age for the mature, but also adding them onto the total .
     */
    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        // We now need to loop through all ages to apply ageing misclassification matrix to account
        // for ages older than max_age_ that could be classified as an individual within the observation range
        unsigned age = ( (*category_iter)->min_age_ + data_offset);

        start_value   = (*cached_category_iter).data_[data_offset];
        end_value     = (*category_iter)->data_[data_offset];

        numbers_age[data_offset] += start_value + (end_value - start_value) * time_step_proportion_;
        total_numbers_age[data_offset] += start_value + (end_value - start_value) * time_step_proportion_;

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << (*category_iter)->name_ << " at age " << age;
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value;
      }
    }

    /*
    *  Apply Ageing error on numbers at age before and after
    */
    if (ageing_error_label_ != "") {
      vector<vector<Double>>& mis_matrix = ageing_error_->mis_matrix();
      vector<Double> temp(numbers_age.size(), 0.0);
      vector<Double> total_temp(total_numbers_age.size(), 0.0);

      for (unsigned i = 0; i < mis_matrix.size(); ++i) {
        for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
          temp[j] += numbers_age[i] * mis_matrix[i][j];
          total_temp[j] += total_numbers_age[i] * mis_matrix[i][j];
        }
      }
      numbers_age = temp;
      total_numbers_age = total_temp;
    }


    /*
     *  Now collapse the number_age into expected values by age
     */
    Double plus = 0, total_plus = 0;
    for (unsigned k = 0; k < numbers_age.size(); ++k) {
      // this is the difference between the
      unsigned age_offset = min_age_ - model_->min_age();
      if (k >= age_offset && (k - age_offset + min_age_) <= max_age_) {
        // not plus group
        if (total_numbers_age[k] > 0.0) {
          expected_values[k - age_offset] = numbers_age[k] / total_numbers_age[k];
          LOG_FINEST() << "age = " << min_age_ + k << " total = " << total_numbers_age[k] << " mature = " << numbers_age[k];
        } else {
          expected_values[k - age_offset] = 0.0;
          LOG_FINEST() << "age = " << k + age_offset << " total = " << total_numbers_age[k] << " mature = " << numbers_age[k];
        }
      }
      if (((k - age_offset + min_age_) > max_age_) && plus_group_) {
        // plus group
        plus += numbers_age[k];
        total_plus += total_numbers_age[k];
      }
    }

    LOG_FINEST() << "Plus group before migration = " << plus << " Plus group after migration = " << total_plus;
    if (plus_group_) {
      if (total_plus > 0.0)
        expected_values[age_spread_ - 1] = plus / total_plus;
      else
        expected_values[age_spread_ - 1] = 0.0;
    }



    if (expected_values.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size("
        << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";
    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values.size(); ++i) {
      LOG_FINEST() << "proportions mature at age " << min_age_ + i << " = " << expected_values[i];
      SaveComparison(category_labels_[category_offset], min_age_ + i ,0.0 ,expected_values[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
          process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsMatureByAge::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
	LOG_FINEST() << "Calculating score for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    /**
     * The comparisons are already proportions so the can be sent straight to the likelihood
     */
    likelihood_->GetScores(comparisons_);

    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "]+ likelihood score: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
