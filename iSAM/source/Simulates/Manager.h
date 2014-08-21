/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIMULATES_MANAGER_H_
#define SIMULATES_MANAGER_H_

// headers
#include "BaseClasses/Manager.h"
#include "Simulates/Simulate.h"

// namespaces
namespace isam {
namespace simulates {

/**
 * Class definition
 */
class Manager : public isam::base::Manager<isam::simulates::Manager, isam::Simulate> {
public:
  // methods
  Manager();
  virtual                     ~Manager() noexcept(true);
  void                        Update(unsigned current_year);
};

} /* namespace simulates */
} /* namespace isam */

#endif /* MANAGER_H_ */
