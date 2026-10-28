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

// Namespaces
namespace niwa::processes::age {

/**
 * Default constructor
 *
 * @param unit Whether this instance's catch/u_max/vulnerable-stock quantity is expressed as
 *   numbers-at-age or biomass -- see RemovalUnit in MortalityEventRemovalBase.h. Supplied by
 *   Factory.cpp at construction time.
 *
 * The two pre-merge classes bound an identical parameter set -- same names, same order, same
 * descriptions -- so there is nothing here that depends on unit_.
 */
MortalityInitialisationEventRemoval::MortalityInitialisationEventRemoval(shared_ptr<Model> model, RemovalUnit unit) :
    MortalityEventRemovalBase(model, "MortalityInitialisationEventRemoval", unit) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories")->flag_is_category();
  parameters_.Bind<Double>(PARAM_CATCH, &catch_, "The amount of removals (catches) to apply for each year");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate ($U_{max}$)")->set_default_value(0.99);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The list of selectivities");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The label of the penalty to apply if the total amount of removals cannot be taken")->set_default_value("");

  RegisterAsAddressable(PARAM_CATCH, &catch_);
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
 * - Build partition, selectivity and penalty references (shared, see MortalityEventRemovalBase)
 */
void MortalityInitialisationEventRemoval::DoBuild() {
  BuildPartitionSelectivitiesAndPenalty();
}

/**
 * Execute the initialisation mortality event process
 */
void MortalityInitialisationEventRemoval::DoExecute() {
  LOG_TRACE();

  // only apply during the initialisation phase
  if (model()->state() != State::kInitialise)
    return;

  /**
   * Work out the exploitation rate to remove (catch/vulnerable), capped at u_max
   */
  Double exploitation = CalculateExploitation(catch_).exploitation;
  if (exploitation < 0.0)
    exploitation = 0.0;
  LOG_FINEST() << "; exploitation: " << AS_DOUBLE(exploitation);

  /**
   * Remove the stock now. The pre-merge mortality_initialisation_event_biomass recomputed the
   * selectivity here instead of reading the vulnerability cache -- it had to, because its cache
   * held biomass while the removal must be in numbers. The cache is now always numbers-based, so
   * cached_numbers_vulnerable * exploitation == data * selectivity * exploitation, with the same
   * operator grouping, and the shared removal loop is bit-for-bit identical to both originals.
   */
  RemoveAtExploitation(exploitation);
}

}  // namespace niwa::processes::age
