/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * This method will look for additional_priors that match the parameter.
 *
 * @param parameter The parameter the additional prior is targeting
 * @return True if found, false otherwise
 */
bool Manager::HasAdditionalPrior(string_view parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if (additional_prior->parameter() == parameter)
      return true;
  }
  return false;
}

/**
 * Get the additional prior pointer
 *
 * @param parameter The parameter string
 * @return An AdditionalPrior pointer
 */
AdditionalPrior* Manager::GetAdditionalPrior(string_view parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if (additional_prior->parameter() == parameter)
      return additional_prior;
  }
  return nullptr;
}
/**
 * This method will look for additional_priors that match the parameter.
 * will ignore additional_priors of type 'ratio'
 *
 * @param parameter The parameter the additional prior is targeting
 * @return True if found, false otherwise
 */
bool Manager::HasAdditionalPriorExcludingRatioType(string_view parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if ((additional_prior->parameter() == parameter) && (additional_prior->type() != PARAM_RATIO)) {
      LOG_FINE() << "param looking for = " << parameter << " this param " << additional_prior->parameter() << " type = " << additional_prior->type();
      return true;
    }
  }
  return false;
}

/**
 * Get the additional prior pointer will ignore additional_priors of type 'ratio'
 *
 * @param parameter The parameter string
 * @return An AdditionalPrior pointer
 */
AdditionalPrior* Manager::GetAdditionalPriorExcludingRatioType(string_view parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if ((additional_prior->parameter() == parameter) && (additional_prior->type() != PARAM_RATIO))
      return additional_prior;
  }
  return nullptr;
}
} /* namespace additionalpriors */
} /* namespace niwa */
