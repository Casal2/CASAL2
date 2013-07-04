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
namespace isam {
namespace derivedquantities {

/**
 * default constructor
 */
Manager::Manager() {
}

/**
 * destructor
 */
Manager::~Manager() noexcept(true) {
}

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

  return DerivedQuantityPtr();
}

/**
 *
 */
vector<DerivedQuantityPtr> Manager::GetForInitialisationTimeStep(const string& label) {
  vector<DerivedQuantityPtr> results;
  for (DerivedQuantityPtr quantity : objects_) {
    if (quantity->IsAssignedToInitialisationPhase(label))
      results.push_back(quantity);
  }

  return results;
}

/**
 * Return a vector of the derived quantities that are assigned to
 * the time step defined by label
 *
 * @param label The label of the time step for the derived quantity
 * @return vector of derived quantities
 */
vector<DerivedQuantityPtr> Manager::GetForTimeStep(const string& label) {
  vector<DerivedQuantityPtr> results;
  for (DerivedQuantityPtr quantity : objects_) {
    if (quantity->time_step() == label)
      results.push_back(quantity);
  }

  return results;
}

} /* namespace derivedquantities */
} /* namespace isam */
