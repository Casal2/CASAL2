/**
 * @file ConstantRecruitment.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "RecruitmentConstant.h"

#include "Utilities/DoubleCompare.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace processes {

using isam::partition::accessors::CategoriesWithAge;

/**
 * Default Constructor
 */
RecruitmentConstant::RecruitmentConstant() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_PROPORTIONS);
  parameters_.RegisterAllowed(PARAM_AGE);
  parameters_.RegisterAllowed(PARAM_R0);
}

/**
 * Validate the parameters for this process
 *
 * 1. Check for the required parameters
 * 2. Assign our label from the parameters
 * 3. Assign remaining local parameters
 */
void RecruitmentConstant::Validate() {

  // Check for the required parameters
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_CATEGORIES);
  CheckForRequiredParameter(PARAM_AGE);
  CheckForRequiredParameter(PARAM_R0);

  // Assign local parameters
  label_           = parameters_.Get(PARAM_LABEL).GetValue<string>();
  category_names_  = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  r0_              = parameters_.Get(PARAM_R0).GetValue<double>();
  age_             = parameters_.Get(PARAM_AGE).GetValue<unsigned>();

  /**
   * Check our parameter proportion is the correct length
   * and sums to 1.0. If it doesn't sum to one we'll make it
   * and print a warning message
   */
  if (parameters_.IsDefined(PARAM_PROPORTIONS)) {
    Parameter parameter = parameters_.Get(PARAM_PROPORTIONS);
    vector<double> proportions = parameters_.Get(PARAM_PROPORTIONS).GetValues<double>();

    if (proportions.size() != category_names_.size()) {
      LOG_ERROR("At line " << parameter.line_number() << " of file " << parameter.file_name()
          << ": Number of proportions provided is not the same as the number of categories provided. Expected: "
          << category_names_.size()<< " but got " << proportions.size());
    }

    double proportion_total = 0.0;

    for (double proportion : proportions)
      proportion_total += proportion;

    if (!utilities::doublecompare::IsOne(proportion_total)) {
      LOG_WARNING("At line " << parameter.line_number() << " of file " << parameter.file_name()
          <<": proportion does not sum to 1.0. Proportion sums to " << proportion_total << ". Auto-scaling proportions to sum to 1.0");

      for (double& proportion : proportions)
        proportion = proportion / proportion_total;
    }

    for (unsigned i = 0; i < category_names_.size(); ++i) {
      proportions_[category_names_[i]] = proportions[i];
    }

  } else {
    // Assign equal proportions to every category
    double proportion = category_names_.size() / 1.0;
    for (string category : category_names_)
      proportions_[category] = proportion;
  }

}

/**
 * Build any runtime relationships we might
 * have to other objects in the system.
 */
void RecruitmentConstant::Build() {
  partition_ = CategoriesWithAgePtr(new CategoriesWithAge(category_names_, age_));
}

/**
 * Execute our constant recruitment process
 */
void RecruitmentConstant::Execute() {
  /**
   * Calculate new proportion totals for categories missing this year
   */
 double total_proportions = 0.0;
 for (auto iterator = partition_->begin(); iterator != partition_->end(); ++iterator) {
    total_proportions += proportions_[iterator->first];
 }

 /**
  * Update our partition with new recruitment values
  */
 for (auto iterator = partition_->begin(); iterator != partition_->end(); ++iterator) {
   *iterator->second += (proportions_[iterator->first] / total_proportions) * r0_;
 }

}

} /* namespace processes */
} /* namespace isam */
