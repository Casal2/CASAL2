/**
 * @file Manager.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
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
#ifndef PROCESSES_MANAGER_H_
#define PROCESSES_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "Processes/Process.h"

// Namespaces
namespace niwa {
class Model;
namespace processes {

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::processes::Manager, niwa::Process> {
  friend class niwa::base::Manager<niwa::processes::Manager, niwa::Process>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Validate() override final;
  void                        Validate(Model* model);
  Process*                    GetProcess(const string& label);

protected:
  // methods
  Manager();
};

} /* namespace processes */
} /* namespace niwa */
#endif /* PROCESSES_MANAGER_H_ */
