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
  AssignLabelFromParameters();

  // Assign local parameters
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
    proportions_ = parameters_.Get(PARAM_PROPORTIONS).GetValues<double>();

    if (proportions_.size() != category_names_.size()) {
      LOG_ERROR("At line " << parameter.line_number() << " of file " << parameter.file_name()
          << ": Number of proportions provided is not the same as the number of categories provided. Expected: "
          << category_names_.size()<< " but got " << proportions_.size());
    }

    double proportion_total = 0.0;
    for (double proportion : proportions_)
      proportion_total += proportion;

    if (!utilities::doublecompare::IsOne(proportion_total)) {
      LOG_WARNING("At line " << parameter.line_number() << " of file " << parameter.file_name()
          <<": proportion does not sum to 1.0. Proportion sums to " << proportion_total << ". Auto-scaling proportions to sum to 1.0");

      for (double& proportion : proportions_)
        proportion = proportion / proportion_total;
    }

  } else {
    // Assign equal proportions to every category
    double proportion = category_names_.size() / 1.0;
    proportions_.assign(category_names_.size(), proportion);

  }

}

/**
 * Build any runtime relationships we might
 * have to other objects in the system.
 */
void RecruitmentConstant::Build() {

}

/**
 *
 */
void RecruitmentConstant::Execute() {

}

} /* namespace processes */
} /* namespace isam */
