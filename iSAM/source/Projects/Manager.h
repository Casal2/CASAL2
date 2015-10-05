/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROJECTS_MANAGER_H_
#define PROJECTS_MANAGER_H_

// headers
#include "BaseClasses/Manager.h"
#include "Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class Manager : public niwa::oldbase::Manager<niwa::projects::Manager, niwa::Project> {
  friend class niwa::oldbase::Manager<niwa::projects::Manager, niwa::Project>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Update(unsigned current_year);

protected:
  // methods
  Manager();
};

} /* namespace projects */
} /* namespace niwa */

#endif /* MANAGER_H_ */
