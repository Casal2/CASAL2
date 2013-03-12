/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
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
#ifndef MANAGER_H_
#define MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Catchabilities/Catchability.h"

// Namespaces
namespace isam {
namespace catchabilities {

/**
 *
 */
class Manager : public isam::base::Manager<isam::catchabilities::Manager, isam::Catchability> {
public:
  // Methods
  Manager();
  virtual                     ~Manager() noexcept(true);
  CatchabilityPtr             GetCatchability(const string& label);
};

} /* namespace catchabilities */
} /* namespace isam */
#endif /* MANAGER_H_ */
