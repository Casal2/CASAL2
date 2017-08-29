/**
 * @file MortalityInitialisationEventBiomass.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 6/4/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2017-06-04 $
 */

// Headers
#include "MortalityInitialisationEventBiomass.h"

#include "Common/Categories/Categories.h"
#include "Common/Penalties/Manager.h"
#include "Common/Selectivities/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/To.h"
#include "Common/TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace age {
namespace processes {

/**
 * Default Constructor
 */
MortalityInitialisationEventBiomass::MortalityInitialisationEventBiomass(Model* model)
  : Process(model),
    partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.Bind<Double>(PARAM_CATCH, &catch_, "The number of removals (catches) to apply for each year", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Maximum exploitation rate ($Umax$)", "", 0.99);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "List of selectivities", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_name_, "The label of the penalty to apply if the total number of removals cannot be taken", "", "");

  RegisterAsAddressable(PARAM_U_MAX, &u_max_);
  RegisterAsAddressable(PARAM_CATCH, &catch_);

  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 * Validate our Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityInitialisationEventBiomass::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  // Validate that the number of selectivities is the same as the number of categories
  if (category_labels_.size() != selectivity_names_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << " Number of selectivities provided does not match the number of categories provided."
        << " Expected " << category_labels_.size() << " but got " << selectivity_names_.size();
  }

  // Validate u_max
  if (u_max_ < 0.0 || u_max_ > 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << ": u_max must be between 0.0 and 1.0 (inclusive). Value defined was " << AS_DOUBLE(u_max_);
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityInitialisationEventBiomass::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  if (penalty_name_ != "") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_name_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_name_ << " does not exist. Have you defined it?";
    }
  }
}

/**
 * Execute our mortality event object.
 *
 */
void MortalityInitialisationEventBiomass::DoExecute() {
  LOG_TRACE();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  // only apply if initialisation phase
  if (model_->state() == State::kInitialise) {
    /**
     * Work our how much of the stock is available or vulnerable to exploit
     */
    Double vulnerable = 0.0;
    unsigned i = 0;
    for (auto categories : partition_) {
      unsigned j = 0;
      //categories->UpdateMeanWeightData();
      for (Double& data : categories->data_) {
        Double temp = data * selectivities_[i]->GetResult(categories->min_age_ + j, categories->age_length_);
        vulnerable += temp * categories->mean_weight_by_time_step_age_[time_step_index][categories->min_age_ + j];
        ++j;
      }
      ++i;
    }
    /**
     * Work out the exploitation rate to remove (catch/vulnerable)
     */
    Double exploitation = 0;
    LOG_FINEST() << "vulnerable biomass = " << vulnerable << " catch = " << catch_;
    exploitation = catch_ / utilities::doublecompare::ZeroFun(vulnerable);
    if (exploitation > u_max_) {
      exploitation = u_max_;
      if (penalty_)
        penalty_->Trigger(label_, catch_, vulnerable*u_max_);

    } else if (exploitation < 0.0) {
      exploitation = 0.0;
    }
    LOG_FINEST() << "; exploitation: " << AS_DOUBLE(exploitation);

    /**
     * Remove the stock now. The amount to remove is
     * vulnerable * exploitation
     */
    // Report catches and exploitation rates for each category for each iteration
/*
    StoreForReport("initialisation_iteration: ", init_iteration_);
    StoreForReport("Exploitation: ", AS_DOUBLE(exploitation));
    StoreForReport("Catch: ", AS_DOUBLE(catch_));
*/
    Double removals =0;
    for (auto categories : partition_) {
      unsigned offset = 0;
      for (Double& data : categories->data_) {
        // report
        removals = vulnerable_[categories->name_][categories->min_age_ + offset] * exploitation;
        //StoreForReport(categories->name_ + "_Removals: ", AS_DOUBLE(removals));
        data -= removals;
        offset++;
      }
    }
    ++init_iteration_;
  }
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
