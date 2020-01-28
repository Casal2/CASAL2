/**
 * @file ProcessRemovalsByAgeRetainedTotal.cpp
 * @author  S Datta
 * @version 1.0
 * @date 02/05/19
 * @section LICENSE
 *
 * Copyright NIWA Science 2019 - www.niwa.co.nz
 */

// Headers
#include "ProcessRemovalsByAgeRetainedTotal.h"

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Model/Model.h"
#include "TimeSteps/Manager.h"
#include "AgeingErrors/Manager.h"
#include "Categories/Categories.h"
#include "Partition/Accessors/All.h"
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
ProcessRemovalsByAgeRetainedTotal::ProcessRemovalsByAgeRetainedTotal(Model* model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Use age plus group", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of time-step that the observation occurs in", "");
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "Tolerance", "", double(0.001))->set_range(0.0, 1.0, false, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "Label of process error to use", "", true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "Label of ageing error to use", "", "");
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "Label of observed method of removals", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "Table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "Table of error values of the observed values (note the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_MORTALITY_INSTANTANEOUS_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation", "");

  mean_proportion_method_ = false;

  RegisterAsAddressable(PARAM_PROCESS_ERRORS, &process_error_values_);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
}

/**
 * Destructor
 */
ProcessRemovalsByAgeRetainedTotal::~ProcessRemovalsByAgeRetainedTotal() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProcessRemovalsByAgeRetainedTotal::DoValidate() {
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
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provided (" << process_error_values_.size()
      << ") does not match the number of years provided (" << years_.size() << ")";
  }
  for (auto year : years_) {
    if((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year()
        << "), or greater than final_year (" << model_->final_year() << ").";
  }
  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }
  if (process_error_values_.size() != 0)
    process_errors_by_year_ = utilities::Map<Double>::create(years_, process_error_values_);
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
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but " << years_.size()
        << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but " << obs_expected
          << " should match the age spread * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_OBS) << " " << obs_by_year[year].size() << " ages were supplied, but " << obs_expected -1 << " ages are required";
  }


  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but " << years_.size()
        << " should match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_FATAL_P(PARAM_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
          << " should match the age spread * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      double value = 0;
      if (!utilities::To<double>(error_values_data_line[i], value))
        LOG_FATAL_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a double";
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
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " " << error_values_by_year[year].size() << " error values by year were provided, but "
        << obs_expected -1 << " values are required based on the obs table";
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
        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end())
        {
          unsigned obs_index = i * age_spread_ + j;
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
  if (time_step_label_.size() != method_.size()) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Specify the same number of time step labels as methods. " << time_step_label_.size()
      << " time step labels have been specified, but there are " << method_.size() << " methods specified";
  }
}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void ProcessRemovalsByAgeRetainedTotal::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));

  // Create a pointer to misclassification matrix
    if( ageing_error_label_ != "") {
    ageing_error_ = model_->managers().ageing_error()->GetAgeingError(ageing_error_label_);
    if (!ageing_error_)
      LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
    }
    if (ageing_error_label_ == "") {
      LOG_WARNING() << "An age-based observation with no ageing_misclassification was provided";
    }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  for (string time_label : time_step_label_) {
    auto time_step = model_->managers().time_step()->GetTimeStep(time_label);
    if (!time_step) {
      LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_label << " was not found.";
    } else {
      auto process = time_step->SubscribeToProcess(this, years_, process_label_);
      mortality_instantaneous_retained_ = dynamic_cast<MortalityInstantaneousRetained*>(process);
    }
  }

  // Need to split the categories if any are combined for checking
  vector<string> temp_split_category_labels, split_category_labels;

  for (const string& category_label : category_labels_) {
    boost::split(temp_split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : temp_split_category_labels) {
      split_category_labels.push_back(split_category_label);
    }
  }
  for (auto category : split_category_labels)
    LOG_FINEST() << category;
  if (!mortality_instantaneous_retained_)
    LOG_ERROR_P(PARAM_PROCESS) << "This observation can be used only for Process of type " << PARAM_MORTALITY_INSTANTANEOUS_RETAINED
      << ". Process " << process_label_ << " was not found.";
  // Do some checks so that the observation and process are compatible
  if (!mortality_instantaneous_retained_->check_methods_for_removal_obs(method_))
    LOG_ERROR_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the methods are compatible with this process";
  if (!mortality_instantaneous_retained_->check_categories_in_methods_for_removal_obs(method_, split_category_labels))
    LOG_ERROR_P(PARAM_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the categories are compatible with this process";
  if (!mortality_instantaneous_retained_->check_years_in_methods_for_removal_obs(years_, method_))
    LOG_ERROR_P(PARAM_YEARS) << "could not find catches with catch in all years in the instantaneous_mortality process labeled " << process_label_
      << ". Check that the years are compatible with this process";

  // If this observation is made up of multiple methods lets find out the last one, because that is when we execute the process
  vector<unsigned> time_step_index;
  for (string label : time_step_label_)
    time_step_index.push_back(model_->managers().time_step()->GetTimeStepIndex(label));

  unsigned last_method_time_step = 9999;
  if (time_step_index.size() > 1) {
    for (unsigned i = 0; i < (time_step_index.size() - 1); ++i) {
      if (time_step_index[i] > time_step_index[i + 1])
        last_method_time_step = time_step_index[i];
      else
        last_method_time_step = time_step_index[i + 1];
    }
    time_step_to_execute_ = last_method_time_step;
  } else {
    time_step_to_execute_ = time_step_index[0];
  }

  LOG_FINEST() << "Executing observation in time step = " << time_step_to_execute_;

}

/**
 * This method is called at the start of the targetted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void ProcessRemovalsByAgeRetainedTotal::PreExecute() {
}

/**
 *
 */
void ProcessRemovalsByAgeRetainedTotal::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  // Check if we are in the final time_step so we have all the relevent information from the Mortaltiy process
  unsigned current_time_step = model_->managers().time_step()->current_time_step();
  if (time_step_to_execute_ == current_time_step) {

    unsigned year = model_->current_year();
    map<unsigned, map<string, map<string, vector<Double>>>> &Removals_at_age = mortality_instantaneous_retained_->catch_at();

    auto partition_iter = partition_->Begin(); // vector<vector<partition::Category> >
    for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
      vector<Double> expected_values(age_spread_, 0.0);
      vector<Double> accumulated_expected_values(age_spread_, 0.0);
      LOG_FINEST() << "Category = " << category_labels_[category_offset];
      auto category_iter = partition_iter->begin();
      for (; category_iter != partition_iter->end(); ++category_iter) {
        // Go through all the fisheries and accumulate the expectation whilst also applying ageing error
        unsigned method_offset = 0;
        for (string fishery : method_) {
          // This should get caught in the DoBuild now.
          if (Removals_at_age.find(year) == Removals_at_age.end() ||
              Removals_at_age[year].find(fishery) == Removals_at_age[year].end() ||
              Removals_at_age[year][fishery].find((*category_iter)->name_) == Removals_at_age[year][fishery].end() ||
              Removals_at_age[year][fishery][(*category_iter)->name_].size() == 0) {
            LOG_FATAL() << "There is no catch at age data in year " << year << " for method " << fishery << " applied to category = " << (*category_iter)->name_
              << ". Check that the mortality_instantaneous process '" << process_label_<< "' is comparable with the observation " << label_;
          }
          /*
           *  Apply Ageing error on Removals at age vector
           */
          if (ageing_error_label_ != "") {
            vector < vector < Double >> &mis_matrix = ageing_error_->mis_matrix();
            vector<Double> temp(Removals_at_age[year][fishery][(*category_iter)->name_].size(), 0.0);
            LOG_FINEST() << "category = " << (*category_iter)->name_;
            LOG_FINEST() << "size = " << Removals_at_age[year][fishery][(*category_iter)->name_].size();

            for (unsigned i = 0; i < mis_matrix.size(); ++i) {
              for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
                temp[j] += Removals_at_age[year][fishery][(*category_iter)->name_][i] * mis_matrix[i][j];
              }
            }
            Removals_at_age[year][fishery][(*category_iter)->name_] = temp;
          }
          LOG_TRACE();
          /*
           *  Now collapse the number_age into the expected_values for the observation
           */
          for (unsigned k = 0; k < Removals_at_age[year][fishery][(*category_iter)->name_].size(); ++k) {
            LOG_FINE() << "----------";
            LOG_FINE() << "Fishery: " << fishery;
            LOG_FINE() << "Numbers At Age After Ageing error: " << (*category_iter)->min_age_ + k << "for category "
              << (*category_iter)->name_ << " " << Removals_at_age[year][fishery][(*category_iter)->name_][k];

            unsigned age_offset = min_age_ - model_->min_age();
            if (k >= age_offset && (k - age_offset + min_age_) <= max_age_)
            expected_values[k - age_offset] = Removals_at_age[year][fishery][(*category_iter)->name_][k];
            // Deal with the plus group
            if (((k - age_offset + min_age_) > max_age_) && plus_group_)
            expected_values[age_spread_ - 1] += Removals_at_age[year][fishery][(*category_iter)->name_][k];
          }

          if (expected_values.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
          LOG_CODE_ERROR()<< "expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size("
            << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

          // Accumulate the expectations if they come form multiple fisheries
          for (unsigned i = 0; i < expected_values.size(); ++i)
          accumulated_expected_values[i] += expected_values[i];

          method_offset++;
        }
      }

      /**
       * save our comparisons so we can use them to generate the score from the likelihoods later
       */
      for (unsigned i = 0; i < expected_values.size(); ++i) {
        LOG_FINEST() << "-----";
        LOG_FINEST() << "Numbers at age for category: " << category_labels_[category_offset] << " for age " << min_age_ + i << " = " << accumulated_expected_values[i];
        SaveComparison(category_labels_[category_offset], min_age_ + i, 0.0, accumulated_expected_values[i],
                       proportions_[model_->current_year()][category_labels_[category_offset]][i],
                       process_errors_by_year_[model_->current_year()],
                       error_values_[model_->current_year()][category_labels_[category_offset]][i],0.0, delta_, 0.0);
      }
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProcessRemovalsByAgeRetainedTotal::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating score for observation = " << label_;

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
      double total = 0.0;
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
      LOG_FINEST() << "-- Observation score calculation";
      LOG_FINEST() << "[" << year << "] Initial Score:" << scores_[year];

      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "]+ likelihood score: "
            << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace age */
} /* namespace niwa */
} /* namespace observations */
