/**
 * @file MortalityRateFormulas.h
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The one-line removal formulas that distinguish MortalityConstantRemovalRate's two
 * formulations (exponential-decay and exploitation-rate), MortalityDiseaseRate, and
 * SurvivalConstantRate from one another are collected here as a plain, non-virtual enum switch
 * rather than a virtual method. Each of those classes' DoExecute() calls this once per data cell
 * (age or length bin), so keeping the dispatch out of a vtable keeps the hot loop as cheap as
 * the original hand-written per-class formula.
 */
#ifndef PROCESSES_COMMON_MORTALITYRATEFORMULAS_H_
#define PROCESSES_COMMON_MORTALITYRATEFORMULAS_H_

// Headers
#include "Logging/Logging.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa::processes::common {

/**
 * Which one-line equation to apply for a removal.
 *
 * kExponentialDecay and kSurvivalExponentialDecay compute the same curve, but keep each
 * original class's operator grouping bit-for-bit distinct (SurvivalConstantRate's rate term is
 * parenthesized differently to MortalityConstantRemovalRate's/MortalityDiseaseRate's) so unit
 * tests that compare exact doubles are unaffected by this consolidation.
 */
enum class RemovalFormulation {
  kExponentialDecay,          // MortalityConstantRemovalRate (mortality_constant_rate), MortalityDiseaseRate
  kSurvivalExponentialDecay,  // SurvivalConstantRate (rate passed in as 1.0 - s)
  kExploitationRate,          // MortalityConstantRemovalRate (mortality_constant_exploitation)
};

/**
 * Calculate the amount removed from a single data cell.
 *
 * @param formulation Which equation to apply
 * @param data The current numbers/biomass in this cell
 * @param rate The per-category rate for this formulation (m, dm, or 1.0 - s)
 * @param selectivity_value The resolved selectivity value for this cell
 * @param multiplier The time-step ratio (or, for MortalityDiseaseRate, the year effect). Typed
 *   as Double (not double): SurvivalConstantRate's ratio and MortalityDiseaseRate's year effect
 *   are both Double-valued, and an autodiff Double (e.g. ADOL-C's adouble) cannot implicitly
 *   narrow to a plain double parameter, even though a plain double widens to Double just fine.
 * @return The amount to remove from data
 */
inline Double CalculateRemoval(RemovalFormulation formulation, Double data, Double rate, Double selectivity_value, Double multiplier) {
  switch (formulation) {
    case RemovalFormulation::kExponentialDecay:
      // MortalityConstantRemovalRate.cpp (mortality_constant_rate) / MortalityDiseaseRate.cpp: data * (1 - exp(-selectivity * rate * multiplier))
      return data * (1.0 - exp(-selectivity_value * rate * multiplier));
    case RemovalFormulation::kSurvivalExponentialDecay:
      // SurvivalConstantRate.cpp: data * (1 - exp(-selectivity * (rate * multiplier)))
      return data * (1.0 - exp(-selectivity_value * (rate * multiplier)));
    case RemovalFormulation::kExploitationRate:
      // MortalityConstantRemovalRate.cpp (mortality_constant_exploitation): data * selectivity * (rate * multiplier)
      return data * selectivity_value * (rate * multiplier);
  }

  LOG_CODE_ERROR() << "Unhandled RemovalFormulation in CalculateRemoval()";
  return 0.0;
}

/**
 * Per-formulation configuration for the classes that expose a RemovalFormulation as a
 * user-selectable process type (MortalityConstantRemovalRate). Everything that varies between
 * formulations of that shape -- bound parameter name/description, selectivity alias labels, and
 * error message text -- lives here, in exactly one place alongside the enum and the formula
 * itself. Adding a new sibling formula to that family means: one enumerator above, one case in
 * CalculateRemoval(), one entry here, and one Factory.cpp registration -- no new class.
 *
 * MortalityDiseaseRate and SurvivalConstantRate are not part of this family (their control flow
 * -- year-gating, per-cell result storage, Age-only scope -- diverges too much to share this
 * table usefully), so kSurvivalExponentialDecay deliberately has no entry.
 */
struct RemovalFormulationSpec {
  string         rate_parameter_name;            // e.g. PARAM_M, PARAM_U
  string         rate_parameter_description;      // e.g. "The mortality rates"
  string         selectivities_description;       // e.g. "The selectivity labels for each category"
  vector<string> selectivity_alias_labels;         // e.g. {PARAM_RELATIVE_M_BY_AGE, PARAM_RELATIVE_M_BY_LENGTH}
  string         selectivity_error_param;          // which parameter a not-found selectivity error reports against
  string         selectivity_error_prefix;         // text preceding the offending label
  string         time_step_size_mismatch_prefix;   // text preceding the input size in the mismatch error
};

/**
 * Look up the RemovalFormulationSpec for a formulation that belongs to the
 * MortalityConstantRemovalRate family.
 */
inline const RemovalFormulationSpec& GetRemovalFormulationSpec(RemovalFormulation formulation) {
  static const RemovalFormulationSpec kExponentialDecaySpec{PARAM_M,
                                                              "The mortality rates",
                                                              "The selectivity labels for each category",
                                                              {PARAM_RELATIVE_M_BY_AGE, PARAM_RELATIVE_M_BY_LENGTH},
                                                              PARAM_RELATIVE_M_BY_AGE,
                                                              ": M-by-age ogive label ",
                                                              "The number of time step proportions ("};
  static const RemovalFormulationSpec kExploitationRateSpec{PARAM_U,
                                                              "The exploitation rates",
                                                              "The list of exploitation by age/length ogive labels for the categories",
                                                              {PARAM_RELATIVE_U_BY_AGE, PARAM_RELATIVE_U_BY_LENGTH},
                                                              PARAM_SELECTIVITIES,
                                                              ": U-by-age/length ogive label ",
                                                              " The number of time step proportions ("};  // leading space preserved verbatim from
                                                                                                           // the original MortalityConstantExploitation text
  switch (formulation) {
    case RemovalFormulation::kExponentialDecay:
      return kExponentialDecaySpec;
    case RemovalFormulation::kExploitationRate:
      return kExploitationRateSpec;
    case RemovalFormulation::kSurvivalExponentialDecay:
      LOG_CODE_ERROR() << "SurvivalConstantRate does not use GetRemovalFormulationSpec()";
      break;
  }

  static const RemovalFormulationSpec kUnreachable{};
  return kUnreachable;
}

}  // namespace niwa::processes::common
#endif /* PROCESSES_COMMON_MORTALITYRATEFORMULAS_H_ */
