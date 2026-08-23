/**
 * @file MortalityRateBase.h
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Shared state and helper logic for the "constant removal rate" family of processes that
 * iterate a partition applying a per-category rate derived from a selectivity:
 * MortalityDiseaseRate and age::SurvivalConstantRate inherit this class directly.
 *
 * MortalityConstantRemovalRate shares the same helper logic (via the static methods below) but
 * cannot inherit from this class, because it must remain dynamic_cast-able to
 * niwa::processes::age::Mortality* (Processes/Manager.cpp relies on this for removal
 * observations) -- see MortalityFisheryAccessorStub, which it inherits instead.
 */
#ifndef PROCESSES_COMMON_MORTALITYRATEBASE_H_
#define PROCESSES_COMMON_MORTALITYRATEBASE_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Common/Mortality/MortalityRateFormulas.h"
#include "Processes/Process.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa::processes::common {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * Class Definition
 */
class MortalityRateBase : public niwa::Process {
public:
  // Methods
  explicit MortalityRateBase(shared_ptr<Model> model);
  virtual ~MortalityRateBase() = default;

  // Shared helpers -- static so MortalityConstantRemovalRate can call
  // them without inheriting this class (see class comment above).
  static vector<niwa::Selectivity*> ResolveSelectivities(shared_ptr<Model> model, ParameterList& parameters, const vector<string>& selectivity_names, const string& error_param,
                                                           const string& error_prefix);
  static map<unsigned, double> BuildTimeStepRatios(shared_ptr<Model> model, ParameterList& parameters, const string& process_label, const vector<double>& ratios_input,
                                                     const string& size_mismatch_prefix, bool exclusive_lower_bound, const string& bound_error_prefix,
                                                     const string& bound_error_suffix);
  static void FillYearsAndTotalRemovalsReportCache(shared_ptr<Model> model, const vector<Double>& total_removals_by_year, ostringstream& cache);
  static void FillYearsAndTotalRemovalsTabularReportCache(shared_ptr<Model> model, const string& label, const vector<Double>& total_removals_by_year, ostringstream& cache,
                                                            bool first_run);

  // Runs the shared "iterate categories, iterate cells, resolve selectivity, apply the removal
  // formula, subtract, accumulate" loop shared by MortalityConstantRemovalRate and
  // age::SurvivalConstantRate. Not used by MortalityDiseaseRate (year-gated, stores per-cell
  // results, so keeps its own loop). Returns the total amount removed.
  static Double ApplyRemovalLoop(accessor::Categories& partition, ProcessProfile process_profile, RemovalFormulation formulation, OrderedMap<string, Double>& rate_by_category,
                                  const vector<niwa::Selectivity*>& selectivities, Double multiplier);

protected:
  // Members
  vector<string>             category_labels_;
  accessor::Categories       partition_;
  vector<string>             selectivity_names_;
  vector<niwa::Selectivity*> selectivities_;
};

}  // namespace niwa::processes::common
#endif /* PROCESSES_COMMON_MORTALITYRATEBASE_H_ */
