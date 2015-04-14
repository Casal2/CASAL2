/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

// namespaces
namespace niwa {
namespace derivedquantities {

/**
 * Return a named derived quantity from our collection
 *
 * @param label The label of the derived quantity
 * @return pointer to quantity, or empty pointer if not found
 */
DerivedQuantityPtr Manager::GetDerivedQuantity(const string& label) {
  for (DerivedQuantityPtr quantity : objects_) {
    if (quantity->label() == label)
      return quantity;
  }

  LOG_WARNING() << "Returning an empty derived quantity because none was found";
  return DerivedQuantityPtr();
}

} /* namespace derivedquantities */
} /* namespace niwa */
