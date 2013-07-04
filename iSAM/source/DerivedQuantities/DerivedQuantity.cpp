/*
 * DerivedQuantity.cpp
 *
 *  Created on: 6/06/2013
 *      Author: Admin
 */

#include "DerivedQuantity.h"

namespace isam {

/**
 * default constructor
 */
DerivedQuantity::DerivedQuantity() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_TIME_STEPS);
  parameters_.RegisterAllowed(PARAM_INITIALIZATION_TIME_STEPS);
}

/**
 *
 */
void DerivedQuantity::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_TIME_STEPS);

  time_step_label_                  = parameters_.Get(PARAM_TIME_STEPS).GetValue<string>();
  initialisation_time_step_labels_  = parameters_.Get(PARAM_INITIALIZATION_TIME_STEPS).GetValues<string>();

  DoValidate();
}

/**
 *
 */
void DerivedQuantity::Calculate() {

}

/**
 *
 */
void DerivedQuantity::CalculateForInitialisationPhase(unsigned initialisation_phase) {

}

/**
 * Check if this derived quantity has the initialisation phase label
 * assigned too it.
 *
 * @param label The label of the initialisation time step to check
 * @return True if assigned to parameter initialisation time step, false otherwise
 */
bool DerivedQuantity::IsAssignedToInitialisationPhase(const string& label) {
  return std::find(initialisation_time_step_labels_.begin(), initialisation_time_step_labels_.end(), label)
    != initialisation_time_step_labels_.end();
}

} /* namespace isam */
