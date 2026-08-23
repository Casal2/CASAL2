/**
 * @file SurvivalConstantRate.cpp
 * @author  You're Name e.g. Craig Marsh
 * @institute NIWA
 * @version 1.0
 * @date date of creation e.g. 17/07/16
 * @licence
 *
 */

// Headers
#include "SurvivalConstantRate.h"

#include <numeric>

#include "Categories/Categories.h"
#include "Processes/Common/Mortality/MortalityRateFormulas.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default Constructor
 */
SurvivalConstantRate::SurvivalConstantRate(shared_ptr<Model> model) : MortalityRateBase(model) {
  LOG_TRACE();
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories")->flag_is_category();
  parameters_.Bind<Double>(PARAM_S, &s_input_, "The survival rates");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &ratios_, "The time step proportions for the survival rate S", "", true)->set_range(0.0, 1.0, false, true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivity labels for each category", "");

  RegisterAsAddressable(PARAM_S, &s_);
}

/**
 * Validate the Survival Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 's' and 'selectivities' if only one value specified
 * - Check s is >= 0
 * - Check the categories are real
 */
void SurvivalConstantRate::DoValidate() {
  parameters_.ValidateVector(PARAM_S)
      ->GreaterThanOrEqualTo(0.0)
      ->LessThanOrEqualTo(1.0)
      ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
      ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->SumToOne();

  s_ = utilities::OrderedMap<string, Double>::create(category_labels_, s_input_);
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build the list of selectivities
 * - Build the ratios for the number of time steps
 */
void SurvivalConstantRate::DoBuild() {
  partition_.Init(category_labels_);

  selectivities_ = ResolveSelectivities(model(), parameters(), selectivity_names_, PARAM_SELECTIVITIES, ": Selectivity label ");

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of S so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  time_step_ratios_ = BuildTimeStepRatios(model(), parameters(), label_, ratios_, "length (", true, "value (", ") must be between 0.0 (exclusive) and 1.0 (inclusive)");
}

/**
 * Execute the process
 */
void SurvivalConstantRate::DoExecute() {
  LOG_FINEST() << "year: " << model()->current_year();

  // get the ratio to apply first
  unsigned time_step = model()->managers()->time_step()->current_time_step();

  LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
  Double ratio = time_step_ratios_[time_step];

  // The removal loop needs a per-category rate map; survival's rate is (1.0 - s), not s
  // directly, so build the transformed map once here (O(categories), not O(cells)) and hand it
  // to the same shared loop MortalityConstantRemovalRate uses.
  OrderedMap<string, Double> survival_rate;
  for (auto& entry : s_) survival_rate[entry.first] = 1.0 - entry.second;

  common::MortalityRateBase::ApplyRemovalLoop(partition_, process_profile_, common::RemovalFormulation::kSurvivalExponentialDecay, survival_rate, selectivities_, ratio);
}

/**
 * Reset the Survival Process
 */
void SurvivalConstantRate::DoReset() {}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
