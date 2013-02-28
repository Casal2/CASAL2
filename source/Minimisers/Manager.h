/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a standard manager for our minisers
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MINIMISERS_MANAGER_H_
#define MINIMISERS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Minimisers/Minimiser.h"

// Namespaces
namespace isam {
namespace minimisers {

/**
 * Class Definition
 */
class Manager : public isam::base::Manager<minimisers::Manager, isam::Minimiser> {
public:
  // Methods
  Manager();
  virtual                     ~Manager() noexcept(true);
};

} /* namespace minimisers */
} /* namespace isam */
#endif /* MANAGER_H_ */
