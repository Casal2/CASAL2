/**
 * @file ProportionsAtAgeForFishery.cpp
 * @author  C Marsh
 * @version 1.0
 * @date 25/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 */

// Headers
#include "ProportionsAtAgeForFishery.h"

#include <algorithm>

#include "Model/Model.h"
#include "TimeSteps/Manager.h"
#include "AgeingErrors/Manager.h"
#include "Partition/Accessors/All.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace observations {

/**
 * Default constructor
 */
ProportionsAtAgeForFishery::ProportionsAtAgeForFishery(Model* model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Use age plus group", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time steps that the fisheries are in", "");
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tolerance_, "Tolerance", "", Double(0.001));
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Year to execute in", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta", "", DELTA);
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "Process error", "", true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "", "");
  parameters_.Bind<string>(PARAM_FISHERY, &fishery_, "Label of fishery the observation is from", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of Observatons", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "", "", false);
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");

  mean_proportion_method_ = false;
}

/**
 * Destructor
 */
ProportionsAtAgeForFishery::~ProportionsAtAgeForFishery() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAgeForFishery::DoValidate() {
  age_spread_ = (max_age_ - min_age_) + 1;
  map<unsigned, vector<Double>> error_values_by_year;
  map<unsigned, vector<Double>> obs_by_year;

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
      Double value = 0;
      if (!utilities::To<Double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << obs_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")" << " at observation " << label_;
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
      Double value = 0;

      if (!utilities::To<Double>(error_values_data_line[i], value))
        LOG_ERROR_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a double";
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
      LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << error_values_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected -1 << ")";
  }

  /**
   * Validate likelihood type
   */
  if (likelihood_type_ != PARAM_LOGNORMAL && likelihood_type_ != PARAM_MULTINOMIAL)
    LOG_ERROR_P(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the proportions at age observation. "
        << "Supported types are " << PARAM_LOGNORMAL << " and " << PARAM_MULTINOMIAL;

  /**
   * Build our proportions and error values for use in the observation
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  Double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    Double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < age_spread_; ++j) {
        unsigned obs_index = i * age_spread_ + j;
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
void ProportionsAtAgeForFishery::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));

  // Create a pointer to misclassification matrix
    if( ageing_error_label_ != "") {
    ageing_error_ = model_->managers().ageing_error()->GetAgeingError(ageing_error_label_);
    if (!ageing_error_)
      LOG_ERROR_P(PARAM_AGEING_ERROR) << "(" << ageing_error_label_ << ") could not be found. Have you defined it?";
    }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  for (string time_label : time_step_label_) {
    auto time_step = model_->managers().time_step()->GetTimeStep(time_label);
    if (!time_step) {
      LOG_FATAL_P(PARAM_TIME_STEP) << time_label << " could not be found. Have you defined it?";
    } else {
      auto process = time_step->SubscribeToProcess(this, years_, process_label_);
      mortality_instantaneous_ = dynamic_cast<MortalityInstantaneous*>(process);
    }
  }

  if (!mortality_instantaneous_)
    LOG_ERROR_P(PARAM_PROCESS) << "This observation can only be used for Process of type = " << PARAM_MORTALITY_INSTANTANEOUS;
}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProportionsAtAgeForFishery::PreExecute() {
}

/**
 *
 */
void ProportionsAtAgeForFishery::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  // Check if we are in the final time_step so we have all the relevent information from the Mortaltiy process

  unsigned current_time_step = model_->managers().time_step()->current_time_step();
  vector<unsigned> time_step_index;

  for (string label : time_step_label_)
    time_step_index.push_back(model_->managers().time_step()->GetTimeStepIndex(label));

  unsigned last_fishery_time_step = 9999;
  if (time_step_index.size() > 1) {
    for (unsigned i = 0; i < (time_step_index.size() - 1); ++i) {
      if (time_step_index[i] > time_step_index[i + 1])
        last_fishery_time_step = time_step_index[i];
      else
        last_fishery_time_step = time_step_index[i + 1];
    }
  }

if ((time_step_label_.size() > 1 && last_fishery_time_step == current_time_step) || time_step_label_.size() == 1) {

  unsigned year = model_->current_year();
  map<unsigned,map<string, map<string, vector<Double>>>> &Removals_at_age = mortality_instantaneous_->catch_at();

  auto partition_iter = partition_->Begin(); // vector<vector<partition::Category> >
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    vector<Double> expected_values(age_spread_, 0.0);
    vector<Double> accumulated_expected_values(age_spread_, 0.0);

    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      // Go through all the fisheries and accumulate the expectation whilst also applying ageing error
      unsigned fishery_offset = 0;
      for (string fishery : fishery_) {

        /*
         *  Apply Ageing error on Removals at age vector
         */

        if (ageing_error_label_ != "") {
          vector < vector < Double >> &mis_matrix = ageing_error_->mis_matrix();
          vector<Double> temp(Removals_at_age[year][fishery][(*category_iter)->name_].size(), 0.0);

          for (unsigned i = 0; i < mis_matrix.size(); ++i) {
            for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
              temp[j] += Removals_at_age[year][fishery][(*category_iter)->name_][i] * mis_matrix[i][j];
            }
          }
          Removals_at_age[year][fishery][(*category_iter)->name_] = temp;
        }

        /*
         *  Now collapse the number_age into the expected_values for the observation
         */
        for (unsigned k = 0; k < Removals_at_age[year][fishery][(*category_iter)->name_].size(); ++k) {
          LOG_FINE() << "----------";
          LOG_FINE() << "Fishery: " << fishery;
          LOG_FINE() << "Numbers At Age After Ageing error: " << (*category_iter)->min_age_ + k << "for category " << (*category_iter)->name_ << " " << Removals_at_age[year][fishery][(*category_iter)->name_][k];

          unsigned age_offset = min_age_ - model_->min_age();
          if (k >= age_offset && (k - age_offset + min_age_) <= max_age_)
          expected_values[k - age_offset] = Removals_at_age[year][fishery][(*category_iter)->name_][k];
          // Deal with the plus group
          if (((k - age_offset + min_age_) > max_age_) && age_plus_)
          expected_values[age_spread_ - 1] += Removals_at_age[year][fishery][(*category_iter)->name_][k];
        }

        if (expected_values.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
        LOG_CODE_ERROR()<< "expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size("
        << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

        // Accumulate the expectations if they come form multiple fisheries
        for (unsigned i = 0; i < expected_values.size(); ++i)
        accumulated_expected_values[i] += expected_values[i];

        fishery_offset++;
      }
    }

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values.size(); ++i) {
      LOG_FINEST() << "-----";
      LOG_FINEST() << "Numbers at age for category: " << category_labels_[category_offset] << " for age " << min_age_ + i << " = " << accumulated_expected_values[i];
      SaveComparison(category_labels_[category_offset], min_age_ + i, 0.0, accumulated_expected_values[i],
      proportions_[model_->current_year()][category_labels_[category_offset]][i], process_errors_by_year_[model_->current_year()],
      error_values_[model_->current_year()][category_labels_[category_offset]][i], delta_, 0.0);
    }
  }
}
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsAtAgeForFishery::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (model_->run_mode() == RunMode::kSimulation) {

    for (auto& iter : comparisons_) {
      Double total_expec = 0.0;
      for (auto& comparison : iter.second)
        total_expec += comparison.expected_;
      for (auto& comparison : iter.second)
        comparison.expected_ /= total_expec;
    }
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

      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation score calculation";
      LOG_FINEST() << "[" << year << "] Initial Score:" << scores_[year];
      likelihood_->GetScores(comparisons_);
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "]+ likelihood score: "
            << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace observations */
} /* namespace niwa */
