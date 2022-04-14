/**
 * @file Abundance.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Abundance.h"

#include "Catchabilities/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace observations {
namespace length {

namespace utils = niwa::utilities;

/**
 * Default constructor
 */
Abundance::Abundance(shared_ptr<Model> model) : Observation(model) {
  obs_table_ = new parameters::Table(PARAM_OBS);

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The label of the catchability coefficient (q)", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities", "", true);
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "The process error", "", Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of observed values and error values", "", false);

  RegisterAsAddressable(PARAM_PROCESS_ERROR, &process_error_value_);

  allowed_likelihood_types_.push_back(PARAM_NORMAL);
  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_PSEUDO);
}

/**
 * Destructor
 */
Abundance::~Abundance() {
  delete obs_table_;
}

/**
 * Validate configuration file parameters
 */
void Abundance::DoValidate() {
  LOG_TRACE();

  if (category_labels_.size() != selectivity_labels_.size() && expected_selectivity_count_ != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Number of selectivities provided (" << selectivity_labels_.size()
                                     << ") is not valid. Specify either the number of category collections (" << category_labels_.size() << ") or "
                                     << "the number of total categories (" << expected_selectivity_count_ << ")";

  // Delta
  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";
  if (process_error_value_ < 0.0)
    LOG_ERROR_P(PARAM_PROCESS_ERROR) << ": process_error (" << AS_DOUBLE(process_error_value_) << ") cannot be less than 0.0";

  // Obs
  unsigned                num_obs       = category_labels_.size();
  unsigned                vals_expected = 1 + num_obs + 1;  // year, observation value(s), error value
  vector<vector<string>>& obs_data      = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but "
                           << "does not match the number of years provided " << years_.size();
  }

  LOG_MEDIUM() << "Number of categories: " << num_obs << ", number of years: " << years_.size() << ", number of observation columns: " << obs_data.size();

  unsigned year      = 0;
  double   obs_value = 0;
  double   err_value = 0;
  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != vals_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but does not match " << vals_expected;
    }

    if (!utilities::To<unsigned>(obs_data_line.front(), year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line.front() << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " year " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i <= num_obs; ++i) {
      if (!utilities::To<double>(obs_data_line[i], obs_value))
        LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[i] << " could not be converted to a Double. It should be the observation value for this line";
      if (obs_value <= 0.0)
        LOG_ERROR_P(PARAM_OBS) << ": observation value " << obs_value << " for year " << year << " cannot be less than or equal to 0.0";
      proportions_by_year_[year].push_back(obs_value);
    }

    if (!utilities::To<double>(obs_data_line.back(), err_value))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line.back() << " could not be converted to a Double. It should be the error value for this line";
    if (err_value <= 0.0)
      LOG_ERROR_P(PARAM_OBS) << ": error value " << err_value << " for year " << year << " cannot be less than or equal to 0.0";
    error_values_by_year_[year] = err_value;
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void Abundance::DoBuild() {
  LOG_TRACE();

  catchability_ = model_->managers()->catchability()->GetCatchability(catchability_label_);
  if (!catchability_)
    LOG_ERROR_P(PARAM_CATCHABILITY) << ": catchability label " << catchability_label_ << " was not found.";

  if (catchability_->type() == PARAM_NUISANCE) {
    nuisance_q_ = true;
    // create a dynamic cast pointer to the nuisance catchability
    nuisance_catchability_ = dynamic_cast<Nuisance*>(catchability_);
    if (!nuisance_catchability_)
      LOG_ERROR_P(PARAM_CATCHABILITY) << ": catchability label " << catchability_label_ << " could not create dynamic cast for nuisance catchability";
  }

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

  if (partition_->category_count() != selectivities_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": number of selectivities provided (" << selectivities_.size() << ") does not match the number "
                                     << "of categories provided (" << partition_->category_count() << ")";
}
/**
 * Reset
 */
void Abundance::DoReset() {
  calculate_nuisance_q_ = true;
}
/**
 * This method is called before any of the processes
 * in the timestep will be executed. This takes data from
 * the partition that would otherwise be lost once it is modified.
 *
 * Build the cache of the cached partition
 * accessor. This accessor will hold the partition state to use
 * during interpolation
 */
void Abundance::PreExecute() {
  cached_partition_->BuildCache();
}

/**
 * Generate the score
 */
void Abundance::Execute() {
  LOG_FINEST() << "Entering observation " << label_;

  Double         expected_total = 0.0;  // value in the model
  vector<string> keys;
  vector<Double> expecteds;
  vector<double> observeds;
  vector<double> error_values;
  vector<Double> process_errors;
  vector<Double> scores;

  Double   selectivity_result = 0.0;
  Double   start_value        = 0.0;
  Double   end_value          = 0.0;
  Double   final_value        = 0.0;
  double   error_value        = 0.0;

  unsigned current_year = model_->current_year();

  // Loop through the obs
  auto cached_partition_iter = cached_partition_->Begin();
  auto partition_iter        = partition_->Begin();  // auto = map<map<string, vector<partition::category&> > >

  if (proportions_by_year_.find(current_year) == proportions_by_year_.end())
    LOG_CODE_ERROR() << "proportions_by_year_[current_year] for year " << current_year << " was not found";
  if (cached_partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_by_year_[current_year].size()";
  if (partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_by_year_[current_year].size()";

  for (unsigned proportions_index = 0; proportions_index < proportions_by_year_[current_year].size(); ++proportions_index, ++partition_iter, ++cached_partition_iter) {
    expected_total = 0.0;

    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (unsigned category_offset = 0; category_iter != partition_iter->end(); ++category_offset, ++cached_category_iter, ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {

        selectivity_result = selectivities_[category_offset]->GetLengthResult(data_offset);
        start_value        = (*cached_category_iter)->cached_data_[data_offset];
        end_value          = (*category_iter)->data_[data_offset];
        final_value        = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else {
          // re-write of std::abs(start_value - end_value) * temp_step_proportion for ADMB
          Double temp = start_value - end_value;
          temp        = temp < 0 ? temp : temp * -1.0;
          final_value = temp * proportion_of_time_;
        }

        expected_total += selectivity_result * final_value;
      }
    }

    /**
     * expected_total is the number of fish the model has for the category across
     */
    if (!nuisance_q_) {
      // If nuisance q then the default value for Q is 1 but this has a null effect on the expectations so I am just skipping it replicate this and get
      // around issues with bounds in the estimation system
      expected_total *= catchability_->q();
    }

    error_value = error_values_by_year_[current_year];

    // Store the values
    keys.push_back(category_labels_[proportions_index]);
    expecteds.push_back(expected_total);
    observeds.push_back(proportions_by_year_[current_year][proportions_index]);
    error_values.push_back(error_value);
    process_errors.push_back(process_error_value_);
  }

  for (unsigned index = 0; index < observeds.size(); ++index)
    SaveComparison(keys[index], expecteds[index], observeds[index], process_errors[index], error_values[index], 0.0, delta_, 0.0);
}

/**
 * Calculate the score
 */
void Abundance::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    // Check if we have a nusiance q or a free q
    if (catchability_->type() == PARAM_NUISANCE) {
      if(  calculate_nuisance_q_ ) {
        nuisance_catchability_->CalculateQ(comparisons_, likelihood_type_);

        // Log out the new q
        LOG_FINE() << "Q = " << nuisance_catchability_->q();
        // Recalculate the expectations by multiplying by the new Q
        for (auto year_iterator = comparisons_.begin(); year_iterator != comparisons_.end(); ++year_iterator) {
          for (obs::Comparison& comparison : year_iterator->second) {
            LOG_FINEST() << "---- Expected before nuisance Q applied = " << comparison.expected_;
            comparison.expected_ *= nuisance_catchability_->q();
            LOG_FINEST() << "---- Expected After nuisance Q applied = " << comparison.expected_;
          }
        }
        calculate_nuisance_q_ = false;
      }
    }

    likelihood_->SimulateObserved(comparisons_);
  } else {
    /**
     * Convert the expected_values
     */
    // Check if we have a nusiance q or a free q
    if (catchability_->type() == PARAM_NUISANCE) {
      nuisance_catchability_->CalculateQ(comparisons_, likelihood_type_);

      // Log out the new q
      LOG_FINE() << "Q = " << nuisance_catchability_->q();
      // Recalculate the expectations by multiplying by the new Q
      for (auto year_iterator = comparisons_.begin(); year_iterator != comparisons_.end(); ++year_iterator) {
        for (obs::Comparison& comparison : year_iterator->second) {
          LOG_FINEST() << "---- Expected before nuisance Q applied = " << comparison.expected_;
          comparison.expected_ *= nuisance_catchability_->q();
          LOG_FINEST() << "---- Expected After nuisance Q applied = " << comparison.expected_;
        }
      }
    }

    likelihood_->GetScores(comparisons_);

    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      for (obs::Comparison comparison : comparisons_[year]) {
        scores_[year] += comparison.score_;
      }
    }
  }
}

}  // namespace age
} /* namespace observations */
} /* namespace niwa */
