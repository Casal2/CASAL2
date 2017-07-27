/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * This method will look for additional_priors that match our parameter.
 *
 * @param parameter The parameter the additional prior is targeting
 * @return True if found, false otherwise
 */
bool Manager::HasAdditionalPrior(const string& parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if (additional_prior->parameter() == parameter)
      return true;
  }
  return false;
}

/**
 *	return An additionalPrior
 */
AdditionalPrior* Manager::GetAdditionalPrior(const string& parameter) {
  for (AdditionalPrior* additional_prior : objects_) {
    if (additional_prior->parameter() == parameter)
      return additional_prior;
  }
  return nullptr;;
}


} /* namespace additionalpriors */
} /* namespace niwa */




