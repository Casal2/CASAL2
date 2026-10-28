/**
 * @file MortalityEventRemoval.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 */

// Headers
#include "MortalityEventRemoval.h"

#include "Utilities/Map.h"

// Namespaces
namespace niwa::processes::age {

/**
 * Default constructor
 *
 * @param unit Whether this instance's catch/u_max/vulnerable-stock quantity is expressed as
 *   numbers-at-age or biomass -- see RemovalUnit in MortalityEventRemovalBase.h. Supplied by
 *   Factory.cpp at construction time.
 */
MortalityEventRemoval::MortalityEventRemoval(shared_ptr<Model> model, RemovalUnit unit) :
    MortalityEventRemovalBase(model, "MortalityEventRemoval", unit) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories")->flag_is_category();
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years in which to apply the mortality process");
  parameters_.Bind<Double>(PARAM_CATCHES, &catches_, "The catches (numbers or biomass, depending on process type) to apply for each year");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate ($U_{max}$)")->set_default_value(0.99);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The list of selectivities");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The label of the penalty to apply if the total number of removals cannot be taken")->set_default_value("");

  RegisterAsAddressable(PARAM_CATCHES, &catch_years_);
}

/**
 * Validate the Mortality Event Removal process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 *
 * The same validation rules apply regardless of unit_: the pre-merge mortality_event class
 * required years in increasing order and allowed u_max of exactly 0.0/1.0; mortality_event_biomass
 * did not require increasing years and rejected u_max of exactly 0.0/1.0. Consolidation
 * normalizes both process types onto mortality_event's (looser, in the u_max case) rules -- see
 * docs/duplicated-process-classes.md Group C plan for the rationale.
 */
void MortalityEventRemoval::DoValidate() {
  parameters_.ValidateVector(PARAM_YEARS)->IsModelYear()->IsInIncreasingOrder()->SameNumberOfElementsAs(PARAM_CATCHES);
  parameters_.ValidateVector(PARAM_CATCHES)->GreaterThanOrEqualTo(0.0);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.Validate(PARAM_U_MAX)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0);

  catch_years_ = utilities::Map::create(years_, catches_);
}

/**
 * Build the runtime relationships required
 * - Build partition, selectivity and penalty references (shared, see MortalityEventRemovalBase)
 */
void MortalityEventRemoval::DoBuild() {
  BuildPartitionSelectivitiesAndPenalty();

  // Pre allocate memory
  exploitation_by_year_.reserve(years_.size());
  actual_catches_.reserve(years_.size());
}

/**
 * Reset the runtime relationships
 */
void MortalityEventRemoval::DoReset() {
  exploitation_by_year_.clear();
  actual_catches_.clear();
}

/**
 * Execute the mortality event removal
 *
 * Normalized: the pre-merge mortality_event class skipped this whole method during the
 * initialisation phase; mortality_event_biomass did not. The guard below now applies to both --
 * see docs/duplicated-process-classes.md Group C plan for the rationale.
 */
void MortalityEventRemoval::DoExecute() {
  LOG_TRACE();
  auto current_year = model()->current_year();
  if (catch_years_.find(current_year) == catch_years_.end())
    return;
  if (model()->state() == State::kInitialise)
    return;

  /**
   * Work out the exploitation rate to remove (catch/vulnerable), capped at u_max
   */
  CatchExploitation result       = CalculateExploitation(catch_years_[current_year]);
  Double            exploitation = result.exploitation;
  actual_catches_.push_back(result.actual_catch);
  exploitation_by_year_.push_back(exploitation);

  // Normalized: mortality_event zeroed exploitation after this LOG_CODE_ERROR (an unreachable
  // "should never happen" path); mortality_event_biomass did not. Unified since the path is
  // unreachable either way -- see docs/duplicated-process-classes.md Group C plan.
  if (exploitation < 0.0) {
    LOG_CODE_ERROR() << "exploitation < 0.0 for process " << label_;
    exploitation = 0.0;
  }
  LOG_FINEST() << "year: " << current_year << "; exploitation: " << AS_DOUBLE(exploitation);

  RemoveAtExploitation(exploitation);
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityEventRemoval::FillReportCache(ostringstream& cache) {
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
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void MortalityEventRemoval::FillTabularReportCache(ostringstream& cache, bool first_run) {
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

}  // namespace niwa::processes::age
