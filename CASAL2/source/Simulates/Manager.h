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
#include "../BaseClasses/Manager.h"
#include "../Simulates/Simulate.h"

// namespaces
namespace niwa {
namespace simulates {

/**
 * Class definition
 */
class Manager : public niwa::base::Manager<niwa::simulates::Manager, niwa::Simulate> {
  friend class niwa::base::Manager<niwa::simulates::Manager, niwa::Simulate>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Update(unsigned current_year);

protected:
  // methods
  Manager();
};

} /* namespace simulates */
} /* namespace niwa */

#endif /* MANAGER_H_ */
