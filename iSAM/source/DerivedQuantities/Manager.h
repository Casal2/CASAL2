/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the manager for the derived quantities
 */
#ifndef DERIVEDQUANTITIES_MANAGER_H_
#define DERIVEDQUANTITIES_MANAGER_H_

// headers
#include "BaseClasses/Manager.h"
#include "DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace niwa {
namespace derivedquantities {

// classes
class Manager : public niwa::oldbase::Manager<derivedquantities::Manager, niwa::DerivedQuantity> {
  friend class niwa::oldbase::Manager<derivedquantities::Manager, niwa::DerivedQuantity>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  DerivedQuantity*            GetDerivedQuantity(const string& label);
};

} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* MANAGER_H_ */
