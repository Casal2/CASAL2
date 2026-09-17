/**
 * @file MortalityInitialisationEventRemoval.cpp
 * @author  C.Marsh
 * @date 6/4/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 */

// Headers
#include "MortalityInitialisationEventRemoval.h"

#include "AgeLengths/AgeLength.h"
#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Math.h"

// Namespaces
namespace niwa::processes::age {

/**
 * Default constructor
 *
 * @param unit Whether this instance's catch/u_max/vulnerable-stock quantity is expressed as
 *   numbers-at-age or biomass -- see RemovalUnit in MortalityEventRemoval.h. Supplied by
 *   Factory.cpp at construction time.
 *
 * The two pre-merge classes bound an identical parameter set -- same names, same order, same
 * descriptions -- so there is nothing here that depends on unit_.
 */
MortalityInitialisationEventRemoval::MortalityInitialisationEventRemoval(shared_ptr<Model> model, RemovalUnit unit) :
    MortalityFisheryAccessorStub(model, "MortalityInitialisationEventRemoval"), unit_(unit), partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories")->flag_is_category();
  parameters_.Bind<Double>(PARAM_CATCH, &catch_, "The amount of removals (catches) to apply for each year");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate ($U_{max}$)")->set_default_value(0.99);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The list of selectivities");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_name_, "The label of the penalty to apply if the total amount of removals cannot be taken")->set_default_value("");

  RegisterAsAddressable(PARAM_CATCH, &catch_);

  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the initialisation mortality event process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 *
 * The same validation rules apply regardless of unit_.
 *
 * Normalized: the pre-merge mortality_initialisation_event rejected a u_max of exactly 0.0 or 1.0
 * (GreaterThan/LessThan); mortality_initialisation_event_biomass accepted it
 * (GreaterThanOrEqualTo/LessThanOrEqualTo). Consolidation normalizes both process types onto the
 * looser, inclusive rule -- matching the same decision made for the same drift in
 * MortalityEventRemoval. See docs/duplicated-process-classes.md Group D plan for the rationale.
 */
void MortalityInitialisationEventRemoval::DoValidate() {
  parameters_.Validate(PARAM_CATCH)->GreaterThan(0.0);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.Validate(PARAM_U_MAX)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0);
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityInitialisationEventRemoval::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model()->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }

  if (penalty_name_ != "") {
    penalty_ = model()->managers()->penalty()->GetProcessPenalty(penalty_name_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": Penalty label " << penalty_name_ << " was not found.";
    }
  }
}

/**
 * Execute the initialisation mortality event process
 */
void MortalityInitialisationEventRemoval::DoExecute() {
  LOG_TRACE();

  // only apply during the initialisation phase
  if (model()->state() != State::kInitialise)
    return;

  unsigned time_step_index = model()->managers()->time_step()->current_time_step();

  /**
   * Work out how much of the stock is vulnerable. The per-age numbers-at-age vulnerability is
   * always cached into vulnerable_ for reuse in the removal loop below -- for kBiomass, the sum
   * used as the exploitation-rate denominator is additionally weighted by mean-weight-at-age, but
   * the cached per-cell value stays numbers-based, since the removal step itself is always
   * numbers * exploitation for both units (matching both original classes' removal formula).
   */
  Double   vulnerable = 0.0;
  unsigned i          = 0;
  for (auto categories : partition_) {
    unsigned j = 0;
    for (Double& data : categories->data_) {
      Double numbers_vulnerable                                = data * selectivities_[i]->GetAgeResult(categories->min_age_ + j, categories->age_length_);
      vulnerable_[categories->name_][categories->min_age_ + j] = numbers_vulnerable;

      vulnerable += (unit_ == RemovalUnit::kBiomass) ? numbers_vulnerable * categories->age_length_->mean_weight(time_step_index, categories->min_age_ + j) : numbers_vulnerable;
      ++j;
    }

    ++i;
  }

  /**
   * Work out the exploitation rate to remove (catch/vulnerable)
   */
  Double exploitation = 0;
  LOG_FINEST() << "vulnerable = " << vulnerable << " catch = " << catch_;
  exploitation = catch_ / utilities::math::ZeroFun(vulnerable);
  if (exploitation > u_max_) {
    exploitation = u_max_;
    if (penalty_)
      penalty_->Trigger(catch_, vulnerable * u_max_);

  } else if (exploitation < 0.0) {
    exploitation = 0.0;
  }
  LOG_FINEST() << "; exploitation: " << AS_DOUBLE(exploitation);

  /**
   * Remove the stock now. The amount to remove is the cached numbers-at-age vulnerability times
   * the exploitation rate.
   *
   * The pre-merge mortality_initialisation_event_biomass recomputed the selectivity here instead
   * of reading its cache -- it had to, because its cache held biomass while the removal must be in
   * numbers. Since the cache above is now always numbers-based, both process types can share this
   * one loop: cached_numbers_vulnerable * exploitation == data * selectivity * exploitation, with
   * the same operator grouping, so this is bit-for-bit identical to both originals.
   */
  for (auto categories : partition_) {
    unsigned offset = 0;
    for (Double& data : categories->data_) {
      Double removals = vulnerable_[categories->name_][categories->min_age_ + offset] * exploitation;
      data -= removals;
      ++offset;
    }
  }
}

}  // namespace niwa::processes::age
