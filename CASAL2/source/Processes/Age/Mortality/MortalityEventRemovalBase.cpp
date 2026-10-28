/**
 * @file MortalityEventRemovalBase.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 */

// Headers
#include "MortalityEventRemovalBase.h"

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
 *   numbers-at-age or biomass. Supplied by Factory.cpp at construction time.
 */
MortalityEventRemovalBase::MortalityEventRemovalBase(shared_ptr<Model> model, const string& label, RemovalUnit unit) :
    MortalityFisheryAccessorStub(model, label), unit_(unit), partition_(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Build the partition reference and resolve the selectivity and penalty labels
 */
void MortalityEventRemovalBase::BuildPartitionSelectivitiesAndPenalty() {
  partition_.Init(category_labels_);

  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model()->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }

  if (penalty_label_ != "") {
    penalty_ = model()->managers()->penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": Penalty label " << penalty_label_ << " was not found.";
    }
  }
}

/**
 * Work out the exploitation rate needed to take catch_amount, capped at u_max
 *
 * The per-age numbers-at-age vulnerability is always cached into vulnerable_ for reuse by
 * RemoveAtExploitation() -- for kBiomass, the sum used as the exploitation-rate denominator is
 * additionally weighted by mean-weight-at-age, but the cached per-cell value stays numbers-based,
 * since the removal step itself is always numbers * exploitation for both units.
 *
 * If the rate exceeds u_max it is capped, the actual catch becomes vulnerable * u_max, and the
 * penalty (if any) is triggered with (catch_amount, vulnerable * u_max).
 *
 * @param catch_amount The catch to take (numbers or biomass, depending on unit_)
 * @return The (capped) exploitation rate and the catch actually taken
 */
MortalityEventRemovalBase::CatchExploitation MortalityEventRemovalBase::CalculateExploitation(Double catch_amount) {
  unsigned time_step_index = model()->managers()->time_step()->current_time_step();

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

  LOG_FINEST() << "vulnerable = " << vulnerable << " catch = " << catch_amount;
  CatchExploitation result;
  result.exploitation = catch_amount / utilities::math::ZeroFun(vulnerable);

  if (result.exploitation > u_max_) {
    result.exploitation = u_max_;
    result.actual_catch = vulnerable * u_max_;
    if (penalty_)
      penalty_->Trigger(catch_amount, vulnerable * u_max_);
  } else {
    result.actual_catch = catch_amount;
  }

  return result;
}

/**
 * Remove the stock. The amount to remove from each cell is the numbers-at-age vulnerability
 * cached by CalculateExploitation() times the exploitation rate.
 *
 * @param exploitation The exploitation rate to apply
 */
void MortalityEventRemovalBase::RemoveAtExploitation(Double exploitation) {
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
