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

#include "Selectivities/Children/Constant.h"


// namespaces
namespace niwa {
namespace selectivities {

/**
 * Default constructor
 */
Manager::Manager() {

}

/**
 *
 */
SelectivityPtr Manager::GetSelectivity(const string& label) {
  SelectivityPtr result;

  for(SelectivityPtr selectivity : objects_) {
    if (selectivity->label() == label) {
      result = selectivity;
      break;
    }
  }

  return result;
}

} /* namespace selectivities */
} /* namespace niwa */
