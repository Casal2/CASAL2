/**
 * @file MortalityRateBase.cpp
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// Headers
#include "MortalityRateBase.h"

#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa::processes::common {

/**
 * Default Constructor
 */
MortalityRateBase::MortalityRateBase(shared_ptr<Model> model) : Process(model), partition_(model) {}

/**
 * Resolve a list of selectivity labels into pointers, logging (against the caller's own
 * parameters, via the passed-in ParameterList) if a label cannot be found.
 *
 * @param model The model, used to look up the selectivity manager
 * @param parameters The calling process's own ParameterList (for error location reporting)
 * @param selectivity_names The selectivity labels to resolve
 * @param error_param Which parameter the not-found error should be reported against
 * @param error_prefix Text preceding the offending label in the not-found error message
 * @return The resolved selectivity pointers, in the same order as selectivity_names
 */
vector<niwa::Selectivity*> MortalityRateBase::ResolveSelectivities(shared_ptr<Model> model, ParameterList& parameters, const vector<string>& selectivity_names,
                                                                   const string& error_param, const string& error_prefix) {
  vector<Selectivity*> selectivities;
  for (string label : selectivity_names) {
    Selectivity* selectivity = model->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_IF_FOR_STREAM(logger::Severity::kError) << parameters.location(error_param) << error_prefix << label << " was not found.";
    selectivities.push_back(selectivity);
  }
  return selectivities;
}

/**
 * Organise the time step ratios. Each time step can apply a different ratio of the rate, so
 * here verify there are enough and re-scale them to 1.0.
 *
 * @param model The model, used to look up the time step manager
 * @param parameters The calling process's own ParameterList (for error location reporting)
 * @param process_label The calling process's own label (to find which time steps it's assigned to)
 * @param ratios_input The user-provided time_step_proportions values (may be empty)
 * @param size_mismatch_prefix Text preceding the input size in the size-mismatch error message
 * @param exclusive_lower_bound Whether 0.0 itself is out of range (SurvivalConstantRate) or not
 * @param bound_error_prefix Text preceding the offending value in the out-of-range error message
 * @param bound_error_suffix Text following the offending value in the out-of-range error message
 * @return A map of time step index to ratio
 */
map<unsigned, double> MortalityRateBase::BuildTimeStepRatios(shared_ptr<Model> model, ParameterList& parameters, const string& process_label, const vector<double>& ratios_input,
                                                             const string& size_mismatch_prefix, bool exclusive_lower_bound, const string& bound_error_prefix,
                                                             const string& bound_error_suffix) {
  map<unsigned, double> time_step_ratios;

  vector<TimeStep*> time_steps = model->managers()->time_step()->ordered_time_steps();
  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
  vector<unsigned> active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(process_label))
      active_time_steps.push_back(i);
  }

  if (ratios_input.size() == 0) {
    for (unsigned i : active_time_steps) time_step_ratios[i] = 1.0;
  } else {
    if (ratios_input.size() != active_time_steps.size())
      LOG_IF_FOR_STREAM(logger::Severity::kError) << parameters.location(PARAM_TIME_STEP_PROPORTIONS) << size_mismatch_prefix << ratios_input.size()
                                                  << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";

    for (double value : ratios_input) {
      bool out_of_range = exclusive_lower_bound ? (value <= 0.0 || value > 1.0) : (value < 0.0 || value > 1.0);
      if (out_of_range)
        LOG_IF_FOR_STREAM(logger::Severity::kError) << parameters.location(PARAM_TIME_STEP_PROPORTIONS) << bound_error_prefix << value << bound_error_suffix;
    }

    for (unsigned i = 0; i < ratios_input.size(); ++i) time_step_ratios[active_time_steps[i]] = ratios_input[i];
  }

  return time_step_ratios;
}

/**
 * Fill the report cache with the standard "years: .../total_removals: ..." dump shared by
 * MortalityConstantRate and MortalityConstantExploitation.
 */
void MortalityRateBase::FillYearsAndTotalRemovalsReportCache(shared_ptr<Model> model, const vector<Double>& total_removals_by_year, ostringstream& cache) {
  cache << "years: ";
  for (auto year : model->years()) cache << year << " ";
  cache << "\ntotal_removals: ";
  for (auto removal : total_removals_by_year) cache << AS_DOUBLE(removal) << " ";
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache with the standard "removals[label][year] ..." dump shared by
 * MortalityConstantRate and MortalityConstantExploitation.
 */
void MortalityRateBase::FillYearsAndTotalRemovalsTabularReportCache(shared_ptr<Model> model, const string& label, const vector<Double>& total_removals_by_year,
                                                                    ostringstream& cache, bool first_run) {
  if (first_run) {
    for (auto year : model->years()) cache << "removals[" << label << "][" << year << "] ";
    cache << REPORT_EOL;
  }

  for (auto removal : total_removals_by_year) cache << AS_DOUBLE(removal) << " ";
  cache << REPORT_EOL;
}

/**
 * Iterate every category and every data cell in the partition, resolve the selectivity value
 * for that cell (branching on Age vs Length, exactly as each caller did inline before this was
 * extracted), apply the removal formula, subtract the result from the cell, and accumulate the
 * total amount removed across the whole partition.
 */
Double MortalityRateBase::ApplyRemovalLoop(accessor::Categories& partition, ProcessProfile process_profile, RemovalFormulation formulation,
                                           OrderedMap<string, Double>& rate_by_category, const vector<niwa::Selectivity*>& selectivities, Double multiplier) {
  unsigned i            = 0;
  Double   total_amount = 0.0;
  for (auto category : partition) {
    Double   rate = rate_by_category[category->name_];
    unsigned j    = 0;
    for (Double& data : category->data_) {
      Double selectivity_value
          = (process_profile == ProcessProfile::kAge) ? selectivities[i]->GetAgeResult(category->min_age_ + j, category->age_length_) : selectivities[i]->GetLengthResult(j);
      Double amount = CalculateRemoval(formulation, data, rate, selectivity_value, multiplier);
      data -= amount;
      total_amount += amount;
      ++j;
    }
    ++i;
  }
  return total_amount;
}

}  // namespace niwa::processes::common
