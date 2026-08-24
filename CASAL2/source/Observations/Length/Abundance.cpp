/**
 * @file Abundance.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
  obs_table_ = parameters_.BindTable(PARAM_OBS, "The table of observed values and error values");
  obs_table_->set_requires_columns(false);

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in");
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "The label of the catchability coefficient (q)");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities");
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_value_, "The process error")->set_default_value(0.0);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations")->set_is_optional(true);

  RegisterAsAddressable(PARAM_PROCESS_ERROR, &process_error_value_);

  allowed_likelihood_types_ = {PARAM_NORMAL, PARAM_LOGNORMAL, PARAM_PSEUDO};
}

/**
 * Validate configuration file parameters
 */
void Abundance::DoValidate() {
  // parameters_.Validate(PARAM_TYPE)->IsInList({PARAM_ABUNDANCE, PARAM_PROCESS_ABUNDANCE});
  // parameters_.Validate(PARAM_PROCESS)->EitherOrDefined(PARAM_TIME_STEP_PROPORTION);
  // parameters_.Validate(PARAM_PROCESS_PROPORTION)->ForbiddenIfDefined(PARAM_TIME_STEP_PROPORTION)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0);
  // parameters_.Validate(PARAM_TIME_STEP_PROPORTION)->RequiredIf(type_ == PARAM_ABUNDANCE)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.Validate(PARAM_DELTA)->GreaterThanOrEqualTo(0.0);
  parameters_.Validate(PARAM_PROCESS_ERROR)->GreaterThanOrEqualTo(0.0);
  parameters_.ValidateVector(PARAM_YEARS)->IsModelYear()->DefaultToAllModelYears()->IsInIncreasingOrder();

  parameters_.ValidateTable(PARAM_OBS)
      ->Rows(years_.size(), "Number of rows in the observation table must match the number of years provided")
      ->Columns(1 + category_labels_.size() + 1, "Expected year, observation values, and error value columns in the observation table")
      ->ColumnIsYear(0, "First column of the observation table must be a model year")
      ->DoubleDataRange(1, category_labels_.size() + 1, "All columns except the first must be a double value (data + error value) for the observation")
      ->GreaterThan(category_labels_.size() + 1, 0.0);

  proportions_by_year_  = obs_table_->MapColumnsToYear<double>(0u, 1u, category_labels_.size());
  error_values_by_year_ = obs_table_->MapColumnToYear<double>(0u, category_labels_.size() + 1u);
  if (proportions_by_year_.empty())
    LOG_CODE_ERROR() << "proportions_by_year_ is empty, this should not happen";
  if (error_values_by_year_.empty())
    LOG_CODE_ERROR() << "error_values_by_year_ is empty, this should not happen";
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void Abundance::DoBuild() {
  LOG_TRACE();

  catchability_ = model()->managers()->catchability()->GetCatchability(catchability_label_);
  if (!catchability_)
    LOG_ERROR_P(PARAM_CATCHABILITY) << ": catchability label " << catchability_label_ << " was not found.";

  if (catchability_->type() == PARAM_NUISANCE) {
    nuisance_q_ = true;
    // create a dynamic cast pointer to the nuisance catchability
    nuisance_catchability_ = dynamic_cast<Nuisance*>(catchability_);
    if (!nuisance_catchability_)
      LOG_ERROR_P(PARAM_CATCHABILITY) << ": catchability label " << catchability_label_ << " could not create dynamic cast for nuisance catchability";
  }

  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model(), category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model(), category_labels_));

  // Build Selectivity pointers
  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model()->managers()->selectivity()->GetSelectivity(label);
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

  Double                   expected_total = 0.0;  // value in the model
  vector<string>           keys;
  vector<Double>           expecteds;
  vector<double>           observeds;
  vector<double>           error_values;
  vector<Double>           process_errors;
  vector<Double>           scores;
  vector<std::set<string>> selectivity_labels;  // labels for each selectivity

  Double selectivity_result = 0.0;
  Double start_value        = 0.0;
  Double end_value          = 0.0;
  Double final_value        = 0.0;
  double error_value        = 0.0;

  unsigned current_year = model()->current_year();

  // Loop through the obs
  auto cached_partition_iter = cached_partition_->Begin();
  auto partition_iter        = partition_->Begin();  // auto = map<map<string, vector<partition::category&> > >

  if (proportions_by_year_.find(current_year) == proportions_by_year_.end())
    LOG_CODE_ERROR() << "proportions_by_year_[current_year] for year " << current_year << " was not found";
  if (cached_partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR() << "cached_partition_->Size() != proportions_by_year_[current_year].size()";
  if (partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_by_year_[current_year].size()";

  unsigned selectivity_offset = 0;
  for (unsigned proportions_index = 0; proportions_index < proportions_by_year_[current_year].size(); ++proportions_index, ++partition_iter, ++cached_partition_iter) {
    expected_total = 0.0;
    std::set<string> selectivity_labels_set;

    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter, ++selectivity_offset) {
      assert(selectivity_offset < selectivities_.size());

      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        selectivity_labels_set.insert(selectivities_[selectivity_offset]->label());
        selectivity_result = selectivities_[selectivity_offset]->GetLengthResult(data_offset);
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
    selectivity_labels.push_back(selectivity_labels_set);
  }

  for (unsigned index = 0; index < observeds.size(); ++index)
    SaveComparison(keys[index], selectivity_labels[index], expecteds[index], observeds[index], process_errors[index], error_values[index], 0.0, delta_, 0.0);
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

  if (model()->run_mode() == RunMode::kSimulation) {
    // Check if we have a nusiance q or a free q
    if (catchability_->type() == PARAM_NUISANCE) {
      if (calculate_nuisance_q_) {
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
    // Check if we have a nuisance q or a free q
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

}  // namespace length
} /* namespace observations */
} /* namespace niwa */
