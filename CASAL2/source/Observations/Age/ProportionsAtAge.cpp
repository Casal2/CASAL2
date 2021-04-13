/**
 * @file ProportionsAtAge.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ProportionsAtAge.h"

#include <algorithm>

#include "Model/Model.h"
#include "Selectivities/Manager.h"
#include "AgeingErrors/AgeingError.h"
#include "AgeingErrors/Manager.h"
#include "Partition/Accessors/All.h"
#include "../../Partition/Accessors/Cached/CombinedCategories.h"
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
ProportionsAtAge::ProportionsAtAge(shared_ptr<Model> model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The minimum age", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The maximum age", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Is the maximum age the age plus group?", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tolerance_, "The tolerance on the constraint that for each year the sum of proportions in each age must equal 1, e.g., if tolerance = 0.1 then 1 - Sum(Proportions) can be as great as 0.1 ", "", Double(0.001))->set_range(0.0, 1.0, false, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years of the observed values", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities", "", true);
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "The label of ageing error to use", "", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "", "", false);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
  allowed_likelihood_types_.push_back(PARAM_DIRICHLET);
  allowed_likelihood_types_.push_back(PARAM_LOGISTIC_NORMAL);

}

/**
 * Destructor
 */
ProportionsAtAge::~ProportionsAtAge() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProportionsAtAge::DoValidate() {
  LOG_TRACE();
  age_spread_ = (max_age_ - min_age_) + 1;

  map<unsigned, vector<Double>> error_values_by_year;
  map<unsigned, vector<Double>> obs_by_year;

  LOG_FINEST() << "category_labels_.size() = " << category_labels_.size();
  if (category_labels_.size() != selectivity_labels_.size() && expected_selectivity_count_ != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Number of selectivities provided (" << selectivity_labels_.size()
      << ") is not valid. Specify either the number of category collections (" << category_labels_.size() << ") or "
      << "the number of total categories (" << expected_selectivity_count_ << ")";
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
  if (process_error_values_.size() != 0) {
    if (process_error_values_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_PROCESS_ERRORS) << "Supply a process error for each year. Values for " << process_error_values_.size()
        << " years were supplied, but " << years_.size() << " years are required";
    }
    process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);
  } else {
    Double process_val = 0.0;
    process_errors_by_year_ = utilities::Map::create(years_, process_val);
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
      Double value = 0.0;
      if (!utilities::To<Double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a Double";
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
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
        << " should match the age spread * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";
    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      Double value = 0.0;
      if (!utilities::To<Double>(error_values_data_line[i], value))
        LOG_ERROR_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a Double";
      if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be equal to or less than 0.0";
      } else if ((likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) || (likelihood_type_ == PARAM_DIRICHLET && value < 0.0)) {
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
  Double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    Double total = 0.0;

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

}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void ProportionsAtAge::DoBuild() {
  LOG_TRACE();
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

  // Build Selectivity pointers
  for(string label : selectivity_labels_) {
    LOG_FINEST() << "label = " << label;
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  // Create a pointer to Ageing error misclassification matrix
  if( ageing_error_label_ != "") {
  ageing_error_ = model_->managers()->ageing_error()->GetAgeingError(ageing_error_label_);
  if (!ageing_error_)
    LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
  }
  age_results_.resize(age_spread_ * category_labels_.size(), 0.0);

  // Find out how many selectivities there are, we can have selectivities for combined categories. this is what I am trying to solve for
  // We can either have 1 selectivity
  // A selectivity for each category_label
  // or a selectivity for each combined category in each category_label (total categories) These are defined by business rules in the DoValidate.

  if (selectivity_labels_.size() > category_labels_.size()) {
    selectivity_for_combined_categories_ = true;
  }

  expected_values_.resize(age_spread_, 0.0);
  numbers_age_.resize((model_->age_spread() + 1), 0.0);
  numbers_at_age_with_error_.resize((model_->age_spread() + 1), 0.0);
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void ProportionsAtAge::PreExecute() {
  LOG_TRACE();
  cached_partition_->BuildCache();

  if (cached_partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_[model->current_year()].size()";
  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 * Execute
 */
void ProportionsAtAge::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;
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
  unsigned selectivity_iter = 0;

  LOG_FINEST() << "Number of categories " << category_labels_.size();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter, ++selectivity_iter) {
    Double      selectivity_result = 0.0;
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;
    Double      final_value        = 0.0;

    fill(numbers_age_.begin(), numbers_age_.end(),0.0);
    fill(expected_values_.begin(), expected_values_.end(),0.0);
    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */
    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      if(selectivity_iter >= selectivities_.size())
        LOG_CODE_ERROR() << "selectivity_iter > selectivities_.size()";

      LOG_FINEST() << "using selectivity = " << selectivities_[selectivity_iter]->label();
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        // We now need to loop through all ages to apply ageing misclassification matrix to account
        // for ages older than max_age_ that could be classified as an individual within the observation range
        unsigned age = ( (*category_iter)->min_age_ + data_offset);
        selectivity_result = selectivities_[selectivity_iter]->GetAgeResult(age, (*category_iter)->age_length_);
        start_value   = (*cached_category_iter).data_[data_offset];
        end_value     = (*category_iter)->data_[data_offset];
        final_value   = 0.0;

        if (mean_proportion_method_) {
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
          numbers_age_[data_offset] += final_value * selectivity_result;
        } else {
          final_value = fabs(start_value - end_value);
          numbers_age_[data_offset] += final_value * selectivity_result;
        }

        LOG_FINE() << "----------";
        LOG_FINE() << "Category: " << (*category_iter)->name_ << " at age " << age;
        LOG_FINE() << "Selectivity: " << selectivities_[selectivity_iter]->label() << ": " << selectivity_result;
        LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
        LOG_FINE() << "Numbers at age before ageing error is applied: " << numbers_age_[data_offset];
      }
      if(selectivity_for_combined_categories_) {
        ++selectivity_iter;
      }
    }

    /*
    *  Apply Ageing error on numbers at age
    */
    if (ageing_error_label_ != "") {
      vector<vector<Double>>& mis_matrix = ageing_error_->mis_matrix();
      fill(numbers_at_age_with_error_.begin(), numbers_at_age_with_error_.end(),0.0);
      for (unsigned i = 0; i < mis_matrix.size(); ++i) {
        for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
          numbers_at_age_with_error_[j] += numbers_age_[i] * mis_matrix[i][j];
        }
      }
      numbers_age_ = numbers_at_age_with_error_;
    }

    /*
     *  Now collapse the number_age into out expected values
     */
    for (unsigned k = 0; k < numbers_age_.size(); ++k) {
      // this is the difference between the
      unsigned age_offset = min_age_ - model_->min_age();
      if (k >= age_offset && (k - age_offset + min_age_) <= max_age_) {
        expected_values_[k - age_offset] = numbers_age_[k];
      }
      if (((k - age_offset + min_age_) > max_age_) && plus_group_) {
        expected_values_[age_spread_ - 1] += numbers_age_[k];

      }
    }


    if (expected_values_.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values_.size(" << expected_values_.size() << ") != proportions_[category_offset].size("
        << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";


    for (unsigned i = 0; i < expected_values_.size(); ++i) {
      LOG_FINEST() << "-----";
      LOG_FINEST() << "Numbers at age for all categories in age " << min_age_ + i << " = " << expected_values_[i];

      SaveComparison(category_labels_[category_offset], min_age_ + i ,0.0 ,expected_values_[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
          process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsAtAge::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    for (auto& iter :  comparisons_) {
      Double total_expec = 0.0;
      for (auto& comparison : iter.second)
        total_expec += comparison.expected_;
      for (auto& comparison : iter.second)
        comparison.expected_ /= total_expec;
    }
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
      LOG_FINEST() << "-- Observation neglogLikelihood calculation " << label_;
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:"<< scores_[year];
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
