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
namespace isam {
namespace derivedquantities {

/**
 * class definition
 */
class Manager : public isam::oldbase::Manager<derivedquantities::Manager, isam::DerivedQuantity> {
  friend class isam::oldbase::Manager<derivedquantities::Manager, isam::DerivedQuantity>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) { };
  DerivedQuantityPtr          GetDerivedQuantity(const string& label);
};

} /* namespace derivedquantities */
} /* namespace isam */
#endif /* MANAGER_H_ */
