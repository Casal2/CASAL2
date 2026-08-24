/**
 * @file ProcessRemovalsByAgeRetained.cpp
 * @author  S Datta
 * @version 1.0
 * @date 02/05/19
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// Headers
#include "ProcessRemovalsByAgeRetained.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeingErrors/Manager.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"
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
ProcessRemovalsByAgeRetained::ProcessRemovalsByAgeRetained(shared_ptr<Model> model) : Observation(model) {
  obs_table_ = parameters_.BindTable(PARAM_OBS, "The table of observed values");
  obs_table_->set_requires_columns(false);
  error_values_table_ = parameters_.BindTable(PARAM_ERROR_VALUES, "The table of error values of the observed values (note that the units depend on the likelihood)");
  error_values_table_->set_requires_columns(false);

  // clang-format off
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The minimum age");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The maximum age");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Is the maximum age the age plus group?")->set_default_value(true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time steps that the observation occurs in");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The label of the process error to use")->set_is_optional(true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "The label of the ageing error to use")->set_default_value("");
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "The label of observed method of removals")->set_default_value("");
  parameters_.Bind<string>(PARAM_MORTALITY_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation");
  parameters_.Bind<bool>(PARAM_SIMULATED_DATA_SUM_TO_ONE, &simulated_data_sum_to_one_, "Whether simulated data is discrete or scaled by totals to be proportions for each year")
    ->set_default_value(true);
  parameters_.Bind<bool>(PARAM_SUM_TO_ONE, &sum_to_one_, "Scale year (row) observed values by the total, so they sum = 1")->set_default_value(false);
  // clang-format on

  mean_proportion_method_ = false;

  RegisterAsAddressable(PARAM_PROCESS_ERRORS, &process_error_values_);

  allowed_likelihood_types_ = {PARAM_LOGNORMAL, PARAM_MULTINOMIAL, PARAM_DIRICHLET, PARAM_DIRICHLET_MULTINOMIAL, PARAM_LOGISTIC_NORMAL};
  allowed_mortality_types_  = {PARAM_MORTALITY_INSTANTANEOUS_RETAINED};
}

/**
 * Validate configuration file parameters
 */
void ProcessRemovalsByAgeRetained::DoValidate() {
  // set up some variables we'll need
  age_spread_                    = (max_age_ - min_age_) + 1;
  unsigned expected_column_count = age_spread_ * category_labels_.size() + 1;  // +1 for the year column

  parameters_.Validate(PARAM_MIN_AGE)->IsAge();
  parameters_.Validate(PARAM_MAX_AGE)->IsAge();
  parameters_.ValidateVector(PARAM_YEARS)->IsModelYear()->DefaultToAllModelYears();
  parameters_.ValidateVector(PARAM_PROCESS_ERRORS)
      ->GreaterThanOrEqualTo(0.0)
      ->ExpandToSameNumberOfElementsAs(PARAM_YEARS)
      ->SameNumberOfElementsAs(PARAM_YEARS)
      ->DefaultValue(0.0, years_.size());
  parameters_.ValidateVector(PARAM_METHOD_OF_REMOVAL)->SameNumberOfElementsAs(PARAM_TIME_STEP);

  parameters_.ValidateTable(PARAM_OBS)
      ->Rows(years_.size(), "Number of rows in the observation table must match the number of years provided")
      ->Columns(expected_column_count, "Expected year, observation values, and error value columns in the observation table")
      ->ColumnIsYear(0, "First column of the observation table must be a model year")
      ->DoubleDataRange(1, expected_column_count - 1, "All columns except the first must be a double value (data + error value) for the observation")
      ->GreaterThanOrEqualToForRange(1u, expected_column_count - 1, 0.0);

  parameters_.ValidateTable(PARAM_ERROR_VALUES)
      ->Rows(years_.size(), "Number of rows in the error values table must match the number of years provided")
      ->ExpandColumnsTo(category_labels_.size(), 1u)
      ->Columns(category_labels_.size() + 1, "Expected year and error value columns in the error values table")
      ->ColumnIsYear(0, "First column of the error values table must be a model year")
      ->DoubleDataRange(1, category_labels_.size(), "All columns except the first must be a double value (error values) for the observation")
      ->GreaterThanForRange(1, category_labels_.size(), 0.0);

  process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);

  proportions_  = obs_table_->MapColumnsToYearAndCategory(category_labels_, 0u, 1u, expected_column_count - 1);
  error_values_ = error_values_table_->MapColumnsToYearAndCategory(category_labels_, 0u, 1u, category_labels_.size());

  if (sum_to_one_) {
    for (auto& year_pair : proportions_) {
      niwa::utilities::map::scale_to_one<string>(year_pair.second);
    }
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void ProcessRemovalsByAgeRetained::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model(), category_labels_));

  // Create a pointer to misclassification matrix
  if (ageing_error_label_ != "") {
    ageing_error_ = model()->managers()->ageing_error()->GetAgeingError(ageing_error_label_);
    if (!ageing_error_)
      LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
  } else {
    LOG_ERROR() << "An age-based observation with no ageing error type was provided";
  }

  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  for (string time_label : time_step_label_) {
    auto time_step = model()->managers()->time_step()->GetTimeStep(time_label);
    if (!time_step) {
      LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_label << " was not found.";
    } else {
      auto process                      = time_step->SubscribeToProcess(this, years_, process_label_);
      mortality_instantaneous_retained_ = dynamic_cast<MortalityInstantaneousRetained*>(process);
    }
  }

  if (mortality_instantaneous_retained_ == nullptr)
    LOG_FATAL() << "Observation " << label_ << " can be used with Process type " << PARAM_MORTALITY_INSTANTANEOUS_RETAINED << " only. Process " << process_label_
                << " was not found or does not have retained catch characteristics specified.";

  // Need to split the categories if any are combined for checking
  vector<string> temp_split_category_labels, split_category_labels;

  for (const string& category_label : category_labels_) {
    boost::split(temp_split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : temp_split_category_labels) {
      split_category_labels.push_back(split_category_label);
    }
  }
  for (auto category : split_category_labels) LOG_FINEST() << category;

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
  for (string label : time_step_label_) time_step_index.push_back(model()->managers()->time_step()->GetTimeStepIndex(label));

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
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void ProcessRemovalsByAgeRetained::PreExecute() {}

/**
 * Execute
 */
void ProcessRemovalsByAgeRetained::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  // Check if we are in the final time_step so we have all the relevent information from the Mortaltiy process
  unsigned current_time_step = model()->managers()->time_step()->current_time_step();
  if (time_step_to_execute_ == current_time_step) {
    unsigned                                                 year            = model()->current_year();
    map<unsigned, map<string, map<string, vector<Double>>>>& Retained_at_age = mortality_instantaneous_retained_->retained_data();  // edited from catch_at

    auto partition_iter = partition_->Begin();  // vector<vector<partition::Category> >
    for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
      vector<Double> expected_values(age_spread_, 0.0);
      vector<Double> accumulated_expected_values(age_spread_, 0.0);
      LOG_FINEST() << "Category = " << category_labels_[category_offset];
      auto category_iter = partition_iter->begin();
      for (; category_iter != partition_iter->end(); ++category_iter) {
        // Go through all the fisheries and accumulate the expectation whilst also applying ageing error
        for (string fishery : method_) {
          // This should get caught in the DoBuild now.
          if (Retained_at_age.find(year) == Retained_at_age.end() || Retained_at_age[year].find(fishery) == Retained_at_age[year].end()
              || Retained_at_age[year][fishery].find((*category_iter)->name_) == Retained_at_age[year][fishery].end()
              || Retained_at_age[year][fishery][(*category_iter)->name_].size() == 0) {
            LOG_FATAL() << "There is no retained catch at age data in year " << year << " for method " << fishery << " applied to category = " << (*category_iter)->name_
                        << ". Check that the mortality_instantaneous process '" << process_label_ << "' is comparable with the observation " << label_;
          }
          /*
           *  Apply Ageing error on Retained at age vector
           */
          if (ageing_error_label_ != "") {
            vector<vector<Double>>& mis_matrix = ageing_error_->mis_matrix();
            vector<Double>          temp(Retained_at_age[year][fishery][(*category_iter)->name_].size(), 0.0);
            LOG_FINEST() << "category = " << (*category_iter)->name_;
            LOG_FINEST() << "size = " << Retained_at_age[year][fishery][(*category_iter)->name_].size();

            for (unsigned i = 0; i < mis_matrix.size(); ++i) {
              for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
                temp[j] += Retained_at_age[year][fishery][(*category_iter)->name_][i] * mis_matrix[i][j];
              }
            }
            Retained_at_age[year][fishery][(*category_iter)->name_] = temp;
          }
          LOG_TRACE();
          /*
           *  Now collapse the number_age into the expected_values for the observation
           */
          for (unsigned k = 0; k < Retained_at_age[year][fishery][(*category_iter)->name_].size(); ++k) {
            LOG_FINE() << "----------";
            LOG_FINE() << "Fishery: " << fishery;
            LOG_FINE() << "Numbers At Age After Ageing error: " << (*category_iter)->min_age_ + k << " for category " << (*category_iter)->name_ << " "
                       << Retained_at_age[year][fishery][(*category_iter)->name_][k];

            unsigned age_offset = min_age_ - model()->min_age();
            if (k >= age_offset && (k - age_offset + min_age_) <= max_age_)
              expected_values[k - age_offset] = Retained_at_age[year][fishery][(*category_iter)->name_][k];
            // Deal with the plus group
            if (((k - age_offset + min_age_) > max_age_) && plus_group_)
              expected_values[age_spread_ - 1] += Retained_at_age[year][fishery][(*category_iter)->name_][k];
          }

          if (expected_values.size() != proportions_[model()->current_year()][category_labels_[category_offset]].size())
            LOG_CODE_ERROR() << "expected_values.size(" << expected_values.size() << ") != proportions_[category_offset].size("
                             << proportions_[model()->current_year()][category_labels_[category_offset]].size() << ")";

          // Accumulate the expectations if they come form multiple fisheries
          for (unsigned i = 0; i < expected_values.size(); ++i) accumulated_expected_values[i] += expected_values[i];
        }
      }

      /**
       * save our comparisons so we can use them to generate the score from the likelihoods later
       */
      for (unsigned i = 0; i < expected_values.size(); ++i) {
        LOG_FINEST() << "-----";
        LOG_FINEST() << "Numbers at age for category: " << category_labels_[category_offset] << " for age " << min_age_ + i << " = " << accumulated_expected_values[i];
        LOG_FINEST() << "error: " << process_errors_by_year_[model()->current_year()];
        LOG_FINEST() << "props: " << proportions_[model()->current_year()][category_labels_[category_offset]][i];

        SaveComparison(category_labels_[category_offset], min_age_ + i, 0.0, accumulated_expected_values[i],
                       proportions_[model()->current_year()][category_labels_[category_offset]][i], process_errors_by_year_[model()->current_year()],
                       error_values_[model()->current_year()][category_labels_[category_offset]][0], 0.0, delta_, 0.0);
      }
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProcessRemovalsByAgeRetained::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model()->run_mode() == RunMode::kSimulation) {
    for (auto& iter : comparisons_) {
      Double total_expec = 0.0;
      for (auto& comparison : iter.second) total_expec += comparison.expected_;
      for (auto& comparison : iter.second) comparison.expected_ /= total_expec;
    }
    likelihood_->SimulateObserved(comparisons_);
    for (auto& iter : comparisons_) {
      double total = 0.0;
      for (auto& comparison : iter.second) total += comparison.observed_;
      if (simulated_data_sum_to_one_) {
        for (auto& comparison : iter.second) comparison.observed_ /= total;
      }
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
}  // namespace observations
}  // namespace niwa
