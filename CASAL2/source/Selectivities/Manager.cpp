/**
 * @file Manager.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 3/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
Manager::Manager() {

}

/**
 * Return a pointer to the selectivity object
 *
 * @param label The label of the selectivity
 * @return a pointer to the Selectivity
 */
Selectivity* Manager::GetSelectivity(const string& label) {
  for(auto selectivity : objects_) {
    if (selectivity->label() == label) {
      return selectivity;
    }
  }

  LOG_MEDIUM() << "Selectivity label '" << label << "' was not found.";
  return nullptr;
}

} /* namespace selectivities */
} /* namespace niwa */
