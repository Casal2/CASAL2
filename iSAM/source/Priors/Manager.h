/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PRIORS_MANAGER_H_
#define PRIORS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class Definition
 */
class Manager : public isam::base::Manager<isam::priors::Manager, isam::Prior> {
public:
  Manager();
  virtual                     ~Manager() noexcept(true);
  PriorPtr                    GetPrior(const string& label);
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_MANAGER_H_ */
