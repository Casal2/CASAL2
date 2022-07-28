/**
 * @file MortalityInitialisationBaranov.cpp
 * @author  C. Marsh
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "MortalityInitialisationBaranov.h"

#include "../../Categories/Categories.h"
#include "../../Penalties/Manager.h"
#include "../../Selectivities/Manager.h"
#include "../../Utilities/Math.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
MortalityInitialisationBaranov::MortalityInitialisationBaranov(shared_ptr<Model> model) : Process(model), partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories", "");
  parameters_.Bind<Double>(PARAM_FISHING_MORTALITY, &f_, "Fishing mortality ", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The list of selectivities", "");

  RegisterAsAddressable(PARAM_FISHING_MORTALITY, &f_);

  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the Mortality Event Process
 *
 * 1. Check for the required parameters
 * 2. Assign any remaining variables
 */
void MortalityInitialisationBaranov::DoValidate() {

  // Validate that the number of selectivities is the same as the number of categories
  if (category_labels_.size() != selectivity_names_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " The number of selectivities provided (" << selectivity_names_.size() << ") does not match the number of categories provided ("
                                     << category_labels_.size() << ").";
  }
}

/**
 * Build the runtime relationships required
 * - Build partition reference
 */
void MortalityInitialisationBaranov::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";

    selectivities_.push_back(selectivity);
  }


  // Pre allocate memory
  actual_catch_ = 0.0;
}

/**
 * Reset the runtime relationships
 */
void MortalityInitialisationBaranov::DoReset() {

}

/**
 * Execute the mortality event object
 */
void MortalityInitialisationBaranov::DoExecute() {
  LOG_TRACE();

  if (model_->state() == State::kInitialise) {
    /**
     * Work our how much of the stock is vulnerable
     */
    unsigned i = 0;
    Double   amount;
    Double   total_amount = 0.0;
    for (auto category : partition_) {
      unsigned j = 0;
      LOG_FINEST() << "category " << category->name_ << "; min_age: " << category->min_age_;
      for (Double& data : category->data_) {
        amount = data * (1 - exp(-selectivities_[i]->GetAgeResult(category->min_age_ + j, category->age_length_) * f_));
        data -= amount;
        total_amount += amount;
        ++j;
      }
      ++i;
    }
    actual_catch_ = total_amount;
  }
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityInitialisationBaranov::FillReportCache(ostringstream& cache) {

  cache << "actual_catch: " << actual_catch_ << REPORT_EOL;
  cache << "fishing_mortality: " << f_ << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void MortalityInitialisationBaranov::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    cache << "actual_catch[" << label_ << "] ";
    cache << "fishing_mortality[" << label_ << "] ";
    cache << REPORT_EOL;
  }

  cache << AS_DOUBLE(actual_catch_) << " ";
  cache << AS_DOUBLE(f_) << " ";
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
