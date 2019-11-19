/**
 * @file ProportionsMigrating.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ProportionsMigrating.h"
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

// Namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
ProportionsMigrating::ProportionsMigrating(Model* model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of time-step that the observation occurs in", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Use age plus group", "", true);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  // TODO:  is tolerance missing?
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "Process error", "", true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "Table of error values of the observed values (note the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");

  mean_proportion_method_ = false;

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
  allowed_likelihood_types_.push_back(PARAM_DIRICHLET);
}

/**
 * Destructor
 */
ProportionsMigrating::~ProportionsMigrating() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProportionsMigrating::DoValidate() {
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
      double value = 0.0;
      if (!utilities::To<double>(error_values_data_line[i], value))
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
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;

        double error_value = error_values_by_year[iter->first][obs_index];
        error_values_[iter->first][category_labels_[i]].push_back(error_value);
        proportions_[iter->first][category_labels_[i]].push_back(iter->second[obs_index]);
      }
    }
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProportionsMigrating::DoBuild() {
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
  } else
    time_step->SubscribeToProcess(this, years_, process_label_);
}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProportionsMigrating::PreExecute() {
  cached_partition_->BuildCache();
  LOG_FINEST() << "Entering observation " << label_;


  if (cached_partition_->Size() != proportions_[model_->current_year()].size()) {
    LOG_MEDIUM() << "Cached size " << cached_partition_->Size() << " partition size = " << proportions_[model_->current_year()].size();
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";

  }
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";

  expected_values_.resize(age_spread_, 0.0);
  numbers_age_before_.resize((model_->age_spread() + 1), 0.0);
  numbers_age_after_.resize((model_->age_spread() + 1), 0.0);
  numbers_age_before_with_ageing_error_.resize(numbers_age_before_.size(), 0.0);
  numbers_age_after_with_ageing_error_.resize(numbers_age_after_.size(), 0.0);
}

/**
 *
 */
void ProportionsMigrating::Execute() {
  LOG_TRACE();

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // vector<vector<partition::Category> >

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  LOG_FINEST() << "Number of categories " << category_labels_.size();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;

    fill(numbers_age_before_.begin(), numbers_age_before_.end(),0.0);
    fill(numbers_age_after_.begin(), numbers_age_after_.end(),0.0);
    fill(expected_values_.begin(), expected_values_.end(),0.0);


    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
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

        numbers_age_before_[data_offset] += start_value;
        numbers_age_after_[data_offset] += end_value;

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
      fill(numbers_age_before_with_ageing_error_.begin(), numbers_age_before_with_ageing_error_.end(),0.0);
      fill(numbers_age_after_with_ageing_error_.begin(), numbers_age_after_with_ageing_error_.end(),0.0);

      for (unsigned i = 0; i < mis_matrix.size(); ++i) {
        for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
          numbers_age_before_with_ageing_error_[j] += numbers_age_before_[i] * mis_matrix[i][j];
          numbers_age_after_with_ageing_error_[j] += numbers_age_after_[i] * mis_matrix[i][j];
        }
      }
      numbers_age_before_ = numbers_age_before_with_ageing_error_;
      numbers_age_after_ = numbers_age_after_with_ageing_error_;
    }


    /*
     *  Now collapse the number_age into out expected values
     */
    Double plus_before = 0, plus_after = 0;
    for (unsigned k = 0; k < numbers_age_before_.size(); ++k) {
      // this is the difference between the
      unsigned age_offset = min_age_ - model_->min_age();
      if (numbers_age_before_[k] > 0) {
        if (k >= age_offset && (k - age_offset + min_age_) <= max_age_) {
          expected_values_[k - age_offset] = (numbers_age_before_[k] - numbers_age_after_[k]) / numbers_age_before_[k];
          LOG_FINEST() << "Numbers before migration = " << numbers_age_before_[k] << " numbers after migration = " << numbers_age_after_[k]
                   << " proportion migrated = " <<   expected_values_[k - age_offset];
        }
        if (((k - age_offset + min_age_) > max_age_) && plus_group_) {
          plus_before += numbers_age_before_[k];
          plus_after += numbers_age_after_[k];
        }
      } else {
          if (k >= age_offset && (k - age_offset + min_age_) <= max_age_)
            expected_values_[k] = 0;
          if (((k - age_offset + min_age_) > max_age_) && plus_group_) {
            plus_before += 0;
            plus_after += 0;
          }
      }
    }
    LOG_FINEST() << "Plus group before migration = " << plus_before << " Plus group after migration = " << plus_after;
    if (plus_group_)
      expected_values_[age_spread_ - 1] = (plus_before - plus_after) / plus_before;


    if (expected_values_.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << expected_values_.size() << ") != proportions_[category_offset].size("
        << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */

    for (unsigned i = 0; i < expected_values_.size(); ++i) {
      LOG_FINEST() << " Numbers at age " << min_age_ + i << " = " << expected_values_[i];
      SaveComparison(category_labels_[category_offset], min_age_ + i ,0.0 ,expected_values_[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
          process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsMigrating::CalculateScore() {
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
