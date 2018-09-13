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
namespace niwa {
namespace selectivities {

/**
 * Class defintiion
 */
class Manager : public niwa::base::Manager<niwa::selectivities::Manager, niwa::Selectivity> {
  friend class niwa::base::Manager<niwa::selectivities::Manager, niwa::Selectivity>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true) {};
  Selectivity*                GetSelectivity(const string& label);

protected:
  // methods
  Manager();
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* SELECTIVITIES_MANAGER_H_ */
