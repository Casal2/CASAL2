/**
 * @file MortalityEventBiomass.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/11/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "MortalityEventBiomass.h"

#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "AgeLengths/AgeLength.h"

#include "Utilities/Math.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
MortalityEventBiomass::MortalityEventBiomass(shared_ptr<Model> model) : Process(model), partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The category labels", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities for each of the categories", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years in which to apply the mortality process", "");
  parameters_.Bind<Double>(PARAM_CATCHES, &catches_, "The biomass of removals (catches) to apply for each year", "");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate, U_max", "", 0.99)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The label of the penalty to apply if the total biomass of removals cannot be taken", "", "");

  RegisterAsAddressable(PARAM_CATCHES, &catch_years_);

  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the objects
 */
void MortalityEventBiomass::DoValidate() {
  if (u_max_ <= 0.0 || u_max_ >= 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << " (" << u_max_ << ") must be between 0.0 and 1.0 exclusive";

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " The number of selectivities provided (" << selectivity_labels_.size() << ") must match the number of "
                                     << "categories provided (" << category_labels_.size() << ")";
  if (years_.size() != catches_.size())
    LOG_ERROR_P(PARAM_CATCHES) << " The number of catches provided (" << catches_.size() << ") must match the number of "
                               << "years provided (" << years_.size() << ")";

  // Validate: catches_ and years_
  for (unsigned i = 0; i < years_.size(); ++i) {
    if (catch_years_.find(years_[i]) != catch_years_.end()) {
      LOG_ERROR_P(PARAM_YEARS) << " year '" << years_[i] << "' has already been specified.";
    }
    catch_years_[years_[i]] = catches_[i];
  }

  // add extra years in to model for things like forward projection
  vector<unsigned> model_years = model_->years();
  for (unsigned year : model_years) {
    if (catch_years_.find(year) == catch_years_.end())
      catch_years_[year] = 0.0;
  }
}

/**
 * Build the objects
 */
void MortalityEventBiomass::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }

  if (penalty_label_ != "") {
    penalty_ = model_->managers()->penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": Penalty label " << penalty_label_ << " was not found.";
    }
  }
  exploitation_by_year_.reserve(years_.size());
  actual_catches_.reserve(years_.size());
}

/**
 * Reset the objects
 */
void MortalityEventBiomass::DoReset() {
  exploitation_by_year_.clear();
  actual_catches_.clear();
}

/**
 * Execute the process
 */
void MortalityEventBiomass::DoExecute() {
  if (catch_years_[model_->current_year()] == 0)
    return;

  unsigned time_step_index = model_->managers()->time_step()->current_time_step();
  LOG_TRACE();

  /**
   * Work our how much of the stock is vulnerable
   */
  Double   vulnerable = 0.0;
  unsigned i          = 0;
  for (auto categories : partition_) {
    // categories->UpdateMeanWeightData();
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      Double temp = data * selectivities_[i]->GetAgeResult(categories->min_age_ + offset, categories->age_length_);
      vulnerable += temp * categories->age_length_->mean_weight(time_step_index, categories->min_age_ + offset);
      ++offset;
    }

    ++i;
  }

  /**
   * Work out the exploitation rate to remove (catch/vulnerable)
   */
  Double exploitation = catch_years_[model_->current_year()] / utilities::math::ZeroFun(vulnerable);
  if (exploitation > u_max_) {
    exploitation = u_max_;
    actual_catches_.push_back(vulnerable * u_max_);
    if (penalty_)
      penalty_->Trigger(catch_years_[model_->current_year()], vulnerable * u_max_);
    exploitation_by_year_.push_back(exploitation);
  } else {
    exploitation_by_year_.push_back(exploitation);
    actual_catches_.push_back(catch_years_[model_->current_year()]);
  }

  if (exploitation < 0.0) {
    LOG_CODE_ERROR() << "exploitation < 0.0 for process " << label_;
  }

  LOG_FINEST() << "year: " << model_->current_year() << "; exploitation: " << AS_DOUBLE(exploitation);

  /**
   * Remove the stock now. The amount to remove is
   * vulnerable * exploitation and store for report
   */
  i               = 0;
  Double removals = 0;
  for (auto categories : partition_) {
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      removals = data * selectivities_[i]->GetAgeResult(categories->min_age_ + offset, categories->age_length_) * exploitation;
      // StoreForReport(categories->name_ + "_Removals: ",AS_DOUBLE(removals));
      data -= removals;
      ++offset;
    }
    ++i;
  }
}

/**
 * Fill the report cache
 *
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityEventBiomass::FillReportCache(ostringstream& cache) {
  cache << "years: ";
  for (auto year : years_) cache << year << " ";
  cache << "\nactual_catches: ";
  for (auto removal : actual_catches_) cache << AS_DOUBLE(removal) << " ";
  cache << "\nexploitation_rate: ";
  for (auto exploit : exploitation_by_year_) cache << AS_DOUBLE(exploit) << " ";
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 *
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void MortalityEventBiomass::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    for (auto year : years_) {
      cache << "actual_catches[" << label_ << "][" << year << "] ";
    }
    for (auto year : years_) {
      cache << "exploitation[" << label_ << "][" << year << "] ";
    }
    cache << REPORT_EOL;
  }
  for (auto removal : actual_catches_) cache << AS_DOUBLE(removal) << " ";
  for (auto exploit : exploitation_by_year_) cache << AS_DOUBLE(exploit) << " ";
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
