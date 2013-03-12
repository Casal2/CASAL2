/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_MANAGER_H_
#define OBSERVATIONS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Observations/Observation.h"

// Namespaces
namespace isam {
namespace observations {

/**
 * Class Definition
 */
class Manager : public isam::base::Manager<observations::Manager, isam::Observation> {
public:
  Manager();
  virtual ~Manager() noexcept(true);
};

} /* namespace observations */
} /* namespace isam */
#endif /* OBSERVATIONS_MANAGER_H_ */
