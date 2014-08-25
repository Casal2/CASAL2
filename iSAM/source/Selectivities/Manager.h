/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef SELECTIVITIES_MANAGER_H_
#define SELECTIVITIES_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class defintiion
 */
class Manager : public isam::base::Manager<isam::selectivities::Manager, isam::Selectivity> {
  friend class isam::base::Manager<isam::selectivities::Manager, isam::Selectivity>;
public:
  // methods
  virtual                     ~Manager() noexcept(true) {};
  SelectivityPtr              GetSelectivity(const string& label);

protected:
  // methods
  Manager();
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* SELECTIVITIES_MANAGER_H_ */
