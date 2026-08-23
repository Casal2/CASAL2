/**
 * @file MortalityConstantRemovalRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityConstantRemovalRate.h"

#include <numeric>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa::processes::common {

/**
 * Default Constructor
 *
 * @param formulation Which removal-rate formula/parameter set this instance represents --
 *   see MortalityRateFormulas.h. Supplied by Factory.cpp at construction time, since there is no
 *   sensible default and the constructor body below needs it immediately to bind the right
 *   parameter.
 */
MortalityConstantRemovalRate::MortalityConstantRemovalRate(shared_ptr<Model> model, RemovalFormulation formulation) :
    MortalityFisheryAccessorStub(model, "MortalityConstantRemovalRate"), formulation_(formulation), spec_(GetRemovalFormulationSpec(formulation)), partition_(model) {
  LOG_TRACE();
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of category labels")->flag_is_category();
  parameters_.Bind<Double>(spec_.rate_parameter_name, &m_input_, spec_.rate_parameter_description);
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &ratios_, "The time step proportions for the removal rate");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, spec_.selectivities_description)->set_alias_labels(spec_.selectivity_alias_labels);

  RegisterAsAddressable(spec_.rate_parameter_name, &m_);

  partition_structure_ = PartitionType::kAge | PartitionType::kLength;
}

/**
 * Validate the Mortality Constant Removal Rate process
 */
void MortalityConstantRemovalRate::DoValidate() {
  LOG_TRACE();
  parameters_.ValidateVector(spec_.rate_parameter_name)
      ->GreaterThanOrEqualTo(0.0)
      ->LessThanOrEqualTo(1.0)
      ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
      ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0)->SumToOne();
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);

  m_ = utilities::OrderedMap<string, Double>::create(category_labels_, m_input_);
}

/**
 * Build any runtime relationships
 */
void MortalityConstantRemovalRate::DoBuild() {
  partition_.Init(category_labels_);
  selectivities_    = MortalityRateBase::ResolveSelectivities(model(), parameters(), selectivity_names_, spec_.selectivity_error_param, spec_.selectivity_error_prefix);
  time_step_ratios_ = MortalityRateBase::BuildTimeStepRatios(model(), parameters(), label_, ratios_, spec_.time_step_size_mismatch_prefix, false, "Time step proportion values (",
                                                             ") must be between 0.0 and 1.0 (inclusive) and sum to one");
  unsigned n_years  = model()->years().size();
  total_removals_by_year_.reserve(n_years);
}

/**
 * Execute the process
 */
void MortalityConstantRemovalRate::DoExecute() {
  LOG_FINEST() << "year: " << model()->current_year();
  unsigned time_step = model()->managers()->time_step()->current_time_step();
  LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
  double ratio = time_step_ratios_[time_step];
  total_removals_by_year_.push_back(MortalityRateBase::ApplyRemovalLoop(partition_, process_profile_, formulation_, m_, selectivities_, ratio));
}

/**
 * Reset the Mortality Process
 */
void MortalityConstantRemovalRate::DoReset() {
  total_removals_by_year_.clear();
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityConstantRemovalRate::FillReportCache(ostringstream& cache) {
  MortalityRateBase::FillYearsAndTotalRemovalsReportCache(model(), total_removals_by_year_, cache);
}

/**
 * Fill the tabular report cache
 *
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 */
void MortalityConstantRemovalRate::FillTabularReportCache(ostringstream& cache, bool first_run) {
  MortalityRateBase::FillYearsAndTotalRemovalsTabularReportCache(model(), label_, total_removals_by_year_, cache, first_run);
}

}  // namespace niwa::processes::common
