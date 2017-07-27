/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the manager for the derived quantities
 */
#ifndef DERIVEDQUANTITIES_MANAGER_H_
#define DERIVEDQUANTITIES_MANAGER_H_

// headers
#include "Common/BaseClasses/Manager.h"
#include "Common/DerivedQuantities/DerivedQuantity.h"
#include "Common/Model/Managers.h"

// namespaces
namespace niwa {
namespace derivedquantities {

// classes
class Manager : public niwa::base::Manager<derivedquantities::Manager, niwa::DerivedQuantity> {
  friend class niwa::base::Manager<derivedquantities::Manager, niwa::DerivedQuantity>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  DerivedQuantity*            GetDerivedQuantity(const string& label);
};

} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* MANAGER_H_ */
