/**
 * @file Observation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Observation.h"

#include "Model/Model.h"

// Namespaces
namespace isam {

/**
 * Default Constructor
 */
Observation::Observation() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
}

/**
 * Destructor
 */
Observation::~Observation() {
}

/**
 * Validate the parameters passed in from the
 * configuration file
 */
void Observation::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_YEAR);
  CheckForRequiredParameter(PARAM_TIME_STEP);
  CheckForRequiredParameter(PARAM_LIKELIHOOD);

  label_                        = parameters_.Get(PARAM_LABEL).GetValue<string>();
  type_                         = parameters_.Get(PARAM_TYPE).GetValue<string>();
  year_                         = parameters_.Get(PARAM_YEAR).GetValue<unsigned>();
  time_step_label_              = parameters_.Get(PARAM_TIME_STEP).GetValue<string>();
  likelihood_label_             = parameters_.Get(PARAM_LIKELIHOOD).GetValue<string>();
  time_step_proportion_         = parameters_.Get(PARAM_TIME_STEP_PROPORTION).GetValue<double>(1.0);
  time_step_proportion_method_  = parameters_.Get(PARAM_TIME_STEP_PROPORTION_METHOD).GetValue<string>(PARAM_MEAN);
  category_labels_              = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  selectivity_labels_           = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    if (likelihood_label_ == PARAM_PSEUDO) {
      CheckForRequiredParameter(PARAM_SIMULATION_LIKELIHOOD);
      simulation_likelihood_label_ = parameters_.Get(PARAM_SIMULATION_LIKELIHOOD).GetValue<string>();
      likelihood_label_ = simulation_likelihood_label_;
    } else {
      simulation_likelihood_label_ = likelihood_label_;
    }
  }

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0");

  string temp = time_step_proportion_method_;
  if (temp != PARAM_MEAN && temp != PARAM_DIFFERENCE)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTION_METHOD) << ": time_step_proportion_method (" << temp <<") must be either difference or mean");

  if (category_labels_.size() != selectivity_labels_.size()) {
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": number of selectivites (" << selectivity_labels_.size()
        << ") does not match the number of categories (" << category_labels_.size() << ")");
  }
}

} /* namespace isam */
