/**
 * @file Biomass.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Biomass.h"

#include "Catchabilities/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace observations {

namespace utils = niwa::utilities;

/**
 * Default constructor
 */
Biomass::Biomass() {
  parameters_.Bind<string>(PARAM_CATCHABILITY, &catchability_label_, "TBA", "");
  parameters_.Bind<string>(PARAM_OBS, &obs_, "Observation values", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute in", "");
  parameters_.Bind<Double>(PARAM_ERROR_VALUE, &error_values_, "The error values to use against the observation values", "");
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Delta value for error values", "", Double(1e-10));
  parameters_.Bind<Double>(PARAM_PROCESS_ERROR, &process_error_, "Process error", "", Double(0.0));

}

/**
 *
 */
void Biomass::DoValidate() {
  LOG_TRACE();

  // Delta
  if (delta_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_DELTA) << ": delta (" << AS_DOUBLE(delta_) << ") cannot be less than 0.0");
  if (process_error_ < 0.0)
    LOG_ERROR(parameters_.location(PARAM_PROCESS_ERROR) << ": process_error (" << AS_DOUBLE(process_error_) << ") cannot be less than 0.0");

  // Obs
  vector<string> obs  = obs_;
  if (obs.size() != category_labels_.size() * years_.size())
    LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs values length (" << obs.size() << ") must match the number of category collections provided ("
        << category_labels_.size() << ") * years (" << years_.size() << ")");


  // Error Value
  if (error_values_.size() == 1 && obs.size() > 1)
    error_values_.assign(obs.size(), error_values_[0]);
  if (error_values_.size() != obs.size())
    LOG_ERROR(parameters_.location(PARAM_ERROR_VALUE) << ": error_value length (" << error_values_.size()
        << ") must be same length as obs (" << obs.size() << ")");

  error_values_by_year_ = utils::MapCreate(years_, error_values_);

  Double value = 0.0;
  for (unsigned i = 0; i < years_.size(); ++i) {
    for (unsigned j = 0; j < category_labels_.size(); ++j) {
      unsigned index = (i * category_labels_.size()) + j;

      if (!utils::To<Double>(obs[index], value))
            LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << obs[index] << " cannot be converted to a double");
          if (value <= 0.0)
            LOG_ERROR(parameters_.location(PARAM_OBS) << ": obs value " << value << " cannot be less than or equal to 0.0");

          proportions_by_year_[years_[i]].push_back(value);
    }
  }

  /**
   * Verify that the likelihood is from the acceptable ones.
   */
  if (likelihood_type_ != PARAM_NORMAL && likelihood_type_ != PARAM_LOGNORMAL && likelihood_type_ != PARAM_PSEUDO)
    LOG_ERROR(parameters_.location(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_type_ << " is not supported by the biomass observation");
}

/**
 *
 */
void Biomass::DoBuild() {
  LOG_TRACE();

  catchability_ = catchabilities::Manager::Instance().GetCatchability(catchability_label_);
  if (!catchability_)
    LOG_ERROR(parameters_.location(PARAM_CATCHABILITY) << ": catchability " << catchability_label_ << " could not be found. Have you defined it?");

  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(category_labels_));

  if (partition_->category_count() != selectivities_.size())
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": number of selectivities provided (" << selectivities_.size() << ") does not match the number "
        "of categories provided (" << partition_->category_count() << ")");
}

/**
 *
 */
void Biomass::PreExecute() {
  cached_partition_->BuildCache();
}

/**
 *
 */
void Biomass::Execute() {

  Double expected_total = 0.0; // value in the model
  vector<string> keys;
  vector<Double> expecteds;
  vector<Double> observeds;
  vector<Double> error_values;
  vector<Double> process_errors;
  vector<Double> scores;

  Double selectivity_result = 0.0;
  Double start_value = 0.0;
  Double end_value = 0.0;
  Double final_value = 0.0;
  unsigned age = 0;
  Double error_value = 0.0;

  unsigned current_year = Model::Instance()->current_year();

  // Loop through the obs
  auto cached_partition_iter = cached_partition_->Begin();
  auto partition_iter = partition_->Begin(); // auto = map<map<string, vector<partition::category&> > >

  if (cached_partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR("cached_partition_->Size() != proportions_.size()");
  if (partition_->Size() != proportions_by_year_[current_year].size())
    LOG_CODE_ERROR("partition_->Size() != proportions_.size()");

  for (unsigned proportions_index = 0; proportions_index < proportions_by_year_[current_year].size(); ++proportions_index, ++partition_iter, ++cached_partition_iter) {
    expected_total = 0.0;

    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (unsigned category_offset = 0; category_iter != partition_iter->end(); ++category_offset, ++cached_category_iter, ++category_iter) {
      (*category_iter)->UpdateWeightData();

      for (unsigned data_offset = 0; data_offset < (*category_iter)->age_data_.size(); ++data_offset) {
        age = (*category_iter)->min_age_ + data_offset;

        selectivity_result = selectivities_[category_offset]->GetResult(age);
        start_value = (*cached_category_iter).age_data_[data_offset];
        end_value = (*category_iter)->age_data_[data_offset];
        final_value = 0.0;

        if (mean_proportion_method_)
          final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        else {
          // re-write of std::abs(start_value - end_value) * temp_step_proportion for ADMB
          Double temp = start_value - end_value;
          temp = temp < 0 ? temp : temp * -1.0;
          final_value = temp * proportion_of_time_;
        }

        expected_total += selectivity_result * final_value * (*category_iter)->weight_data_[age];
      }
    }

    /**
     * expected_total is the number of fish the model has for the category across
     */
    expected_total *= catchability_->q();
    error_value = error_values_by_year_[current_year];

    // Store the values
    keys.push_back(category_labels_[proportions_index]);
    expecteds.push_back(expected_total);
    observeds.push_back(proportions_by_year_[current_year][proportions_index]);
    error_values.push_back(error_value);
    process_errors.push_back(process_error_);
  }

  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(keys, observeds, expecteds, error_values,
        process_errors, delta_);
    for (unsigned index = 0; index < observeds.size(); ++index)
      SaveComparison(keys[index], expecteds[index], observeds[index], process_errors[index],
          error_values[index], delta_, 0.0);

  } else {
    likelihood_->GetResult(scores, expecteds, observeds, error_values,
        process_errors, delta_);
    for (unsigned index = 0; index < scores.size(); ++index) {
      scores_[current_year] += scores[index];
      SaveComparison(keys[index], expecteds[index], observeds[index],
          process_errors[index], error_values[index], delta_, scores[index]);
    }
  }
}


} /* namespace observations */
} /* namespace niwa */
